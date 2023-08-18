#!/usr/bin/python3

#
# Copyright (C) 2017-2021, Brown University, Secure Systems Lab.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of Brown University nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

import sys
import os
import json
from seccomp import SeccompFilterContext

class BPFInst:
    def __init__(self, n, jt, jf, pred):
        self.n = n
        self.jt = jt
        self.jf = jf
        self.pred = pred

    def format_inst(self):
        if self.pred == 'ge':
            return "BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, {}, {}, {}),".format(
                self.n, self.jt, self.jf)
        elif self.pred == 'eq':
            return "BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, {}, {}, {}),".format(
                self.n, self.jt, self.jf)
        else:
            raise ValueError

def gen_bpf_array(nums, prog):
    s = 0
    if len(nums) <= 2:
        if len(nums) == 2:
            prog.append(BPFInst(nums[1], 'ok', 0, 'eq'))
            s += 1
        if len(nums) != 0:
            prog.append(BPFInst(nums[0], 'ok', 'ct', 'eq'))
            s += 1
        return s
    else:
        c = len(prog)
        n = len(nums)
        prog.append(BPFInst(nums[n//2], 0, 0, 'ge'))
        s += gen_bpf_array(nums[:n//2], prog)
        prog[c].jt = s
        s += gen_bpf_array(nums[n//2:], prog)
        if s+1 > 256:
            raise ValueError('Total insns greater than 256')
        return s+1

def gen_bpf_prog(nums):
    text = 'BPF_STMT(BPF_LD | BPF_W | BPF_ABS,\n'\
           '(offsetof(struct seccomp_data, nr))),\n'
    while len(nums) > 0:
        curs, nums = nums[:128], nums[128:]
        prog = []
        gen_bpf_array(curs, prog)
        for i, l in enumerate(reversed(prog)):
            if l.jt == 'ok':
                l.jt = i
            if l.jf == 'ct':
                l.jf = i+1
        text += '\n'.join([l.format_inst() for
                           l in prog])
        text += '\nBPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),\n'
    text += 'BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),\n'
    return text


if __name__ == '__main__':
    ctx = SeccompFilterContext()
    ctx.generate_seccomp_lib(gen_bpf_prog)


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
import argparse

class SeccompFilterContext:
    def __init__(self):
        p = argparse.ArgumentParser()
        # p.add_argument('--method', dest='method',
        #                nargs=1, choices=['linear', 'binary', 'nop'],
        #                default='binary')
        p.add_argument('--spec-allow', action='store_const',
                       const=True, default=False,
                       help='enable spec flag')
        p.add_argument('policies', nargs='+', help='policy files')
        self.args = p.parse_args()

    def generate_seccomp_lib(self, gen_bpf_prog, overriding_policy=None):
        policies = self.args.policies
        spec_flag = 'SECCOMP_FILTER_FLAG_SPEC_ALLOW' if self.args.spec_allow else '0'

        text = 'BPF_STMT(BPF_LD | BPF_W | BPF_ABS,\n'\
               '         (offsetof(struct seccomp_data, arch))),\n'\
               'BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),\n'\
               'BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),\n'
        if overriding_policy:
            nums = overriding_policy
        else:
            nums = set()
            for policy in policies:
                #print(policy)
                with open(policy) if policy != "-" else sys.stdin as f:
                    s = f.read()
                    json_in = json.loads(s)
                    if isinstance(json_in, list):
                        syscalls = json_in
                    elif isinstance(json_in, dict):
                        if "syscalls" not in json_in:
                            raise ValueError("Malformed input, key 'syscalls' not present in JSON")
                        syscalls = json_in["syscalls"]
                    else:
                        raise ValueError("Unable to parse input!")

                    nums.update(syscalls)

        nums = sorted(list(nums))
        text += gen_bpf_prog(nums)
        base_dir = os.path.dirname(os.path.realpath(__file__))
        with open(os.path.join(base_dir, 'filter_template.c')) as f:
            tfile = f.read()
            print(tfile.replace('{seccomp_filter}', text).replace('{flag}', spec_flag))

if __name__ == '__main__':
    ctx = SeccompFilterContext()
    ctx.generate_seccomp_lib(lambda n: 'BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),\n',
                             overriding_policy=range(320))

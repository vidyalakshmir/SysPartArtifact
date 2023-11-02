#Sysfilter

Sysfilter can be obtained from https://gitlab.com/Egalito/sysfilter

##Step 1 : Obtaining the source code of sysfilter
Clone the repository of sysfilter in the same directory as SysPartArtifact.
```
git clone --recursive https://gitlab.com/Egalito/sysfilter.git
```
##Step 2 : Making some changes

### Changes to usedef.cpp in Egalito

While running the benchmark applications with sysfilter, we observed a segmentation fault. To fix this, add the following two lines of code to extraction/egalito/src/analysis/usedef.cpp.
```
diff --git a/src/analysis/usedef.cpp b/src/analysis/usedef.cpp
--- a/src/analysis/usedef.cpp
+++ b/src/analysis/usedef.cpp
@@ -1952,6 +1952,8 @@ void UseDef::fillMov(UDState *state, AssemblyPtr assembly) {
 }
 void UseDef::fillMovabs(UDState *state, AssemblyPtr assembly) {
     auto mode = assembly->getAsmOperands()->getMode();
+    if(assembly->getId() == X86_INS_MOVABS)
+        mode = AssemblyOperands::MODE_IMM_REG;
     assert(mode == AssemblyOperands::MODE_IMM_REG);
     if(mode == AssemblyOperands::MODE_IMM_REG) {
         fillImmToReg(state, assembly);
```

### Obtaining system calls filtered with different benchmark server applications
```
cd extraction
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o bind.json $BIND
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o lighttpd.json $LIGHTTPD
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o httpd.json $HTTPD
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o memcached.json $MEMCACHED
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o nginx.json $NGINX
app/sysfilter_extract --disable-nss --dl-autoload --arg-mode-passes=all --full-json -o redis.json $REDIS
```



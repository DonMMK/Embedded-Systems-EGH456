# invoke SourceDir generated makefile for uartecho.pem4f
uartecho.pem4f: .libraries,uartecho.pem4f
.libraries,uartecho.pem4f: package/cfg/uartecho_pem4f.xdl
	$(MAKE) -f /Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI/src/makefile.libs

clean::
	$(MAKE) -f /Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI/src/makefile.libs clean


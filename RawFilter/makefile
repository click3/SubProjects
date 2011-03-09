cc  = cl
!if $(RELEASE) == y
	opt = /errorReport:none /we4715 /W2 /MT /Ox /DBOOST_DISABLE_ASSERTS
!else
	opt = /errorReport:none /we4715 /W2 /MT /Ox
!endif
obj = RawFilter.obj
exe = RawFilter.exe

.c.obj:
	$(cc) /c $(opt) $<
.cpp.obj:
	$(cc) /c $(opt) $<
$(exe).exe: $(obj)
	$(cc) $(opt) /Fe$* $** $(lib)
	mt -manifest $(*).manifest -outputresource:$(*);\#1

RawFilter.obj: RawFilter.c

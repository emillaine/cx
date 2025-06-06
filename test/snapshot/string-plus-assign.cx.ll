
%StringBuffer = type { %"List<char>" }
%"List<char>" = type { ptr, i32, i32 }
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define i32 @main() {
  %a = alloca %StringBuffer, align 8
  %__str = alloca %string, align 8
  call void @_EN3std12StringBuffer4initE(ptr %a)
  %a.load = load %StringBuffer, ptr %a, align 8
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 0)
  %__str.load = load %string, ptr %__str, align 8
  %1 = call %StringBuffer @_EN3stdplE12StringBuffer6string(%StringBuffer %a.load, %string %__str.load)
  store %StringBuffer %1, ptr %a, align 8
  call void @_EN3std12StringBuffer6deinitE(ptr %a)
  ret i32 0
}

declare void @_EN3std12StringBuffer4initE(ptr)

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

declare %StringBuffer @_EN3stdplE12StringBuffer6string(%StringBuffer, %string)

declare void @_EN3std12StringBuffer6deinitE(ptr)

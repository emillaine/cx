
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [4 x i8] c"foo\00", align 1

define i32 @main() {
  %s = alloca %string, align 8
  %__str = alloca %string, align 8
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 3)
  %__str.load = load %string, ptr %__str, align 8
  store %string %__str.load, ptr %s, align 8
  ret i32 0
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

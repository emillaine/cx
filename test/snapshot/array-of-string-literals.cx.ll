
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"bar\00", align 1

define i32 @main() {
  %a = alloca [2 x %string], align 8
  %__str = alloca %string, align 8
  %__str1 = alloca %string, align 8
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 3)
  %__str.load = load %string, ptr %__str, align 8
  %1 = insertvalue [2 x %string] undef, %string %__str.load, 0
  call void @_EN3std6string4initEP4char3int(ptr %__str1, ptr @1, i32 3)
  %__str.load2 = load %string, ptr %__str1, align 8
  %2 = insertvalue [2 x %string] %1, %string %__str.load2, 1
  store [2 x %string] %2, ptr %a, align 8
  ret i32 0
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

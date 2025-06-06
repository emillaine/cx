
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [3 x i8] c"\\n\00", align 1

define i32 @main() {
  %__str = alloca %string, align 8
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 2)
  ret i32 0
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

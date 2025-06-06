
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define i32 @main() {
  %__str = alloca %string, align 8
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 0)
  %1 = call i32 @_EN3std6string4sizeE(ptr %__str)
  ret i32 %1
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

declare i32 @_EN3std6string4sizeE(ptr)

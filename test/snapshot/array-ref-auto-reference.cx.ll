
%"ArrayRef<int>" = type { ptr, i32 }

define void @_EN4main3fooE8ArrayRefI3intE(%"ArrayRef<int>" %a) {
  ret void
}

define i32 @main() {
  %a = alloca [3 x i32], align 4
  store [3 x i32] [i32 1, i32 2, i32 3], ptr %a, align 4
  %1 = getelementptr inbounds [3 x i32], ptr %a, i32 0, i32 0
  %2 = insertvalue %"ArrayRef<int>" undef, ptr %1, 0
  %3 = insertvalue %"ArrayRef<int>" %2, i32 3, 1
  call void @_EN4main3fooE8ArrayRefI3intE(%"ArrayRef<int>" %3)
  call void @_EN4main3bazEPA3_3int(ptr %a)
  ret i32 0
}

define void @_EN4main3bazEPA3_3int(ptr %b) {
  ret void
}

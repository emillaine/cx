
%"Optional<ArrayRef<int8>>" = type { i1, %"ArrayRef<int8>" }
%"ArrayRef<int8>" = type { ptr, i32 }

define i32 @main() {
  %a = alloca %"Optional<ArrayRef<int8>>", align 8
  %1 = alloca %"Optional<ArrayRef<int8>>", align 8
  call void @_EN3std8OptionalI8ArrayRefI4int8EE4initE(ptr %1)
  %.load = load %"Optional<ArrayRef<int8>>", ptr %1, align 8
  store %"Optional<ArrayRef<int8>>" %.load, ptr %a, align 8
  ret i32 0
}

define void @_EN3std8OptionalI8ArrayRefI4int8EE4initE(ptr %this) {
  %hasValue = getelementptr inbounds %"Optional<ArrayRef<int8>>", ptr %this, i32 0, i32 0
  store i1 false, ptr %hasValue, align 1
  ret void
}

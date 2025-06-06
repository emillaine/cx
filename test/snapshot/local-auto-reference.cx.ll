
%Foo = type { i32 }

define i32 @main() {
  %f = alloca %Foo, align 8
  %rf = alloca ptr, align 8
  %pf = alloca ptr, align 8
  store ptr %f, ptr %rf, align 8
  store ptr %f, ptr %pf, align 8
  ret i32 0
}

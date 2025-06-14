
%U = type <{ i32 }>

define i32 @_EN4main3fooE1U(%U %u) {
  %b = extractvalue %U %u, 0
  ret i32 %b
}

define i32 @main() {
  %u = alloca %U, align 8
  store i32 21, ptr %u, align 4
  %u.load = load %U, ptr %u, align 1
  %1 = call i32 @_EN4main3fooE1U(%U %u.load)
  %b.load = load i32, ptr %u, align 4
  %2 = add i32 %1, %b.load
  ret i32 %2
}

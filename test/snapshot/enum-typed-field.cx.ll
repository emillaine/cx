
%S = type { i32, i32 }

define i32 @_EN4main1fE1S(%S %s) {
  %f = extractvalue %S %s, 1
  ret i32 %f
}

define i32 @_EN4main1gEP1S(ptr %s) {
  %f = getelementptr inbounds %S, ptr %s, i32 0, i32 1
  %f.load = load i32, ptr %f, align 4
  ret i32 %f.load
}

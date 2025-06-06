
%Y = type {}

define void @_EN4main1fE1YP1Y(%Y %a, ptr %b) {
  call void @_EN4main1Y6deinitE(ptr %b)
  store %Y %a, ptr %b, align 1
  ret void
}

define void @_EN4main1Y6deinitE(ptr %this) {
  ret void
}

define void @_EN4main1gE1Y(%Y %a) {
  %b = alloca %Y, align 8
  store %Y %a, ptr %b, align 1
  call void @_EN4main1Y6deinitE(ptr %b)
  ret void
}

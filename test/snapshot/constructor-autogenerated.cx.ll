
%X = type { i32, ptr }
%Empty = type {}
%"Generic<float>" = type { float }
%"Generic<Empty>" = type { %Empty }

define i32 @main() {
  %b = alloca i32, align 4
  %x = alloca %X, align 8
  %y = alloca %X, align 8
  %e = alloca %Empty, align 8
  %g = alloca %"Generic<float>", align 8
  %h = alloca %"Generic<Empty>", align 8
  store i32 2, ptr %b, align 4
  call void @_EN4main1X4initE3intP3int(ptr %x, i32 4, ptr %b)
  call void @_EN4main1X4initE3intP3int(ptr %y, i32 4, ptr %b)
  call void @_EN4main5Empty4initE(ptr %e)
  call void @_EN4main7GenericI5floatE4initE5float(ptr %g, float 4.500000e+00)
  %e.load = load %Empty, ptr %e, align 1
  call void @_EN4main7GenericI5EmptyE4initE5Empty(ptr %h, %Empty %e.load)
  call void @_EN4main7GenericI5EmptyE6deinitE(ptr %h)
  ret i32 0
}

define void @_EN4main1X4initE3intP3int(ptr %this, i32 %a, ptr %b) {
  %a1 = getelementptr inbounds %X, ptr %this, i32 0, i32 0
  store i32 %a, ptr %a1, align 4
  %b2 = getelementptr inbounds %X, ptr %this, i32 0, i32 1
  store ptr %b, ptr %b2, align 8
  ret void
}

define void @_EN4main5Empty4initE(ptr %this) {
  ret void
}

define void @_EN4main7GenericI5floatE4initE5float(ptr %this, float %i) {
  %i1 = getelementptr inbounds %"Generic<float>", ptr %this, i32 0, i32 0
  store float %i, ptr %i1, align 4
  ret void
}

define void @_EN4main7GenericI5EmptyE4initE5Empty(ptr %this, %Empty %i) {
  %i1 = getelementptr inbounds %"Generic<Empty>", ptr %this, i32 0, i32 0
  store %Empty %i, ptr %i1, align 1
  ret void
}

define void @_EN4main7GenericI5EmptyE6deinitE(ptr %this) {
  %i = getelementptr inbounds %"Generic<Empty>", ptr %this, i32 0, i32 0
  call void @_EN4main5Empty6deinitE(ptr %i)
  ret void
}

define void @_EN4main5Empty6deinitE(ptr %this) {
  ret void
}


%"C<int>" = type {}
%"C<bool>" = type {}

define i32 @main() {
  %i = alloca %"C<int>", align 8
  %b = alloca %"C<bool>", align 8
  call void @_EN4main1CI3intE4initE(ptr %i)
  call void @_EN4main1CI4boolE4initE(ptr %b)
  call void @_EN4main1CI4boolE6deinitE(ptr %b)
  call void @_EN4main1CI3intE6deinitE(ptr %i)
  ret i32 0
}

define void @_EN4main1CI3intE4initE(ptr %this) {
  ret void
}

define void @_EN4main1CI4boolE4initE(ptr %this) {
  ret void
}

define void @_EN4main1CI4boolE6deinitE(ptr %this) {
  ret void
}

define void @_EN4main1CI3intE6deinitE(ptr %this) {
  ret void
}

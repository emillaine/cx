
%"S<int>" = type { ptr }
%"S<bool>" = type { ptr }

define i32 @main() {
  %1 = alloca %"S<int>", align 8
  %2 = alloca %"S<bool>", align 8
  call void @_EN4main1SI3intE4initE(ptr %1)
  call void @_EN4main1SI4boolE4initE(ptr %2)
  ret i32 0
}

define void @_EN4main1SI3intE4initE(ptr %this) {
  %p = getelementptr inbounds %"S<int>", ptr %this, i32 0, i32 0
  store ptr null, ptr %p, align 8
  ret void
}

define void @_EN4main1SI4boolE4initE(ptr %this) {
  %p = getelementptr inbounds %"S<bool>", ptr %this, i32 0, i32 0
  store ptr null, ptr %p, align 8
  ret void
}


%A = type { %B, %B, i32 }
%B = type { i32 }
%C = type { %B, %B }

declare i1 @f()

define i32 @main() {
  %a = alloca %A, align 8
  %c = alloca %C, align 8
  call void @_EN4main1A4initE(ptr %a)
  call void @_EN4main1C4initE(ptr %c)
  call void @_EN4main1C6deinitE(ptr %c)
  call void @_EN4main1A6deinitE(ptr %a)
  ret i32 0
}

define void @_EN4main1A4initE(ptr %this) {
  ret void
}

define void @_EN4main1C4initE(ptr %this) {
  ret void
}

define void @_EN4main1C6deinitE(ptr %this) {
  %b = getelementptr inbounds %C, ptr %this, i32 0, i32 0
  %bb = getelementptr inbounds %C, ptr %this, i32 0, i32 1
  call void @_EN4main1B6deinitE(ptr %bb)
  call void @_EN4main1B6deinitE(ptr %b)
  ret void
}

define void @_EN4main1A6deinitE(ptr %this) {
  %b = getelementptr inbounds %A, ptr %this, i32 0, i32 0
  %bb = getelementptr inbounds %A, ptr %this, i32 0, i32 1
  %1 = call i1 @f()
  br i1 %1, label %if.then, label %if.else

if.then:                                          ; preds = %0
  call void @_EN4main1B6deinitE(ptr %bb)
  call void @_EN4main1B6deinitE(ptr %b)
  ret void

if.else:                                          ; preds = %0
  br label %if.end

if.end:                                           ; preds = %if.else
  call void @_EN4main1B6deinitE(ptr %bb)
  call void @_EN4main1B6deinitE(ptr %b)
  ret void
}

define void @_EN4main1B6deinitE(ptr %this) {
  ret void
}

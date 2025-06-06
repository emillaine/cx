
define void @_EN4main1fEP3intOP3int(ptr %foo, ptr %bar) {
  %1 = icmp eq ptr %foo, %bar
  br i1 %1, label %if.then, label %if.else

if.then:                                          ; preds = %0
  br label %if.end

if.else:                                          ; preds = %0
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

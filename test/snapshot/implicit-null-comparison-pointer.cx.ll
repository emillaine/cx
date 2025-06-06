
define i32 @main() {
  %foo = alloca ptr, align 8
  %b = alloca i1, align 1
  %a = alloca i32, align 4
  %foo.load = load ptr, ptr %foo, align 8
  %1 = icmp eq ptr %foo.load, null
  store i1 %1, ptr %b, align 1
  %foo.load1 = load ptr, ptr %foo, align 8
  %2 = icmp ne ptr %foo.load1, null
  br i1 %2, label %if.then, label %if.else

if.then:                                          ; preds = %0
  br label %if.end

if.else:                                          ; preds = %0
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %loop.condition

loop.condition:                                   ; preds = %loop.body, %if.end
  %foo.load2 = load ptr, ptr %foo, align 8
  %3 = icmp ne ptr %foo.load2, null
  br i1 %3, label %loop.body, label %loop.end

loop.body:                                        ; preds = %loop.condition
  br label %loop.condition

loop.end:                                         ; preds = %loop.condition
  %foo.load3 = load ptr, ptr %foo, align 8
  %4 = icmp ne ptr %foo.load3, null
  br i1 %4, label %if.then4, label %if.else5

if.then4:                                         ; preds = %loop.end
  br label %if.end6

if.else5:                                         ; preds = %loop.end
  br label %if.end6

if.end6:                                          ; preds = %if.else5, %if.then4
  %if.result = phi i32 [ 1, %if.then4 ], [ 2, %if.else5 ]
  store i32 %if.result, ptr %a, align 4
  ret i32 0
}


%"X<int>" = type { ptr }
%"Y<int>" = type { i32 }

@0 = private unnamed_addr constant [64 x i8] c"Unwrap failed at optional-type-unknown-identifier-bug.cx:11:18\0A\00", align 1

define i32 @main() {
  %x = alloca %"X<int>", align 8
  call void @_EN4main1XI3intE4initE(ptr %x)
  call void @_EN4main1XI3intE6deinitE(ptr %x)
  ret i32 0
}

define void @_EN4main1XI3intE4initE(ptr %this) {
  %y = getelementptr inbounds %"X<int>", ptr %this, i32 0, i32 0
  store ptr null, ptr %y, align 8
  ret void
}

define void @_EN4main1XI3intE6deinitE(ptr %this) {
  %a = alloca i32, align 4
  %y = getelementptr inbounds %"X<int>", ptr %this, i32 0, i32 0
  %y.load = load ptr, ptr %y, align 8
  %assert.condition = icmp eq ptr %y.load, null
  br i1 %assert.condition, label %assert.fail, label %assert.success

assert.fail:                                      ; preds = %0
  call void @_EN3std10assertFailEP4char(ptr @0)
  unreachable

assert.success:                                   ; preds = %0
  %a1 = getelementptr inbounds %"Y<int>", ptr %y.load, i32 0, i32 0
  %a.load = load i32, ptr %a1, align 4
  store i32 %a.load, ptr %a, align 4
  ret void
}

declare void @_EN3std10assertFailEP4char(ptr)

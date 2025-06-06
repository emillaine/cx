
@0 = private unnamed_addr constant [33 x i8] c"Unwrap failed at unwrap.cx:6:37\0A\00", align 1
@1 = private unnamed_addr constant [32 x i8] c"Unwrap failed at unwrap.cx:8:8\0A\00", align 1

declare ptr @f()

define i32 @main() {
  %byte = alloca ptr, align 8
  %ptr = alloca ptr, align 8
  %1 = call ptr @f()
  %assert.condition = icmp eq ptr %1, null
  br i1 %assert.condition, label %assert.fail, label %assert.success

assert.fail:                                      ; preds = %0
  call void @_EN3std10assertFailEP4char(ptr @0)
  unreachable

assert.success:                                   ; preds = %0
  store ptr %1, ptr %byte, align 8
  %2 = call ptr @f()
  store ptr %2, ptr %ptr, align 8
  %ptr.load = load ptr, ptr %ptr, align 8
  %assert.condition1 = icmp eq ptr %ptr.load, null
  br i1 %assert.condition1, label %assert.fail2, label %assert.success3

assert.fail2:                                     ; preds = %assert.success
  call void @_EN3std10assertFailEP4char(ptr @1)
  unreachable

assert.success3:                                  ; preds = %assert.success
  %3 = getelementptr inbounds [1 x i8], ptr %ptr.load, i32 0, i32 0
  store i8 1, ptr %3, align 1
  ret i32 0
}

declare void @_EN3std10assertFailEP4char(ptr)

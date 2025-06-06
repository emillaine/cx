
@0 = private unnamed_addr constant [55 x i8] c"Unwrap failed at pointer-to-array-of-pointers.cx:8:21\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

declare ptr @b()

define i32 @main() {
  %s = alloca ptr, align 8
  %i = alloca i32, align 4
  %1 = call ptr @b()
  store ptr %1, ptr %s, align 8
  store i32 0, ptr %i, align 4
  %s.load = load ptr, ptr %s, align 8
  %assert.condition = icmp eq ptr %s.load, null
  br i1 %assert.condition, label %assert.fail, label %assert.success

assert.fail:                                      ; preds = %0
  call void @_EN3std10assertFailEP4char(ptr @0)
  unreachable

assert.success:                                   ; preds = %0
  %i.load = load i32, ptr %i, align 4
  %2 = getelementptr inbounds ptr, ptr %s.load, i32 %i.load
  %.load = load ptr, ptr %2, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @1, ptr %.load)
  ret i32 0
}

declare void @_EN3std10assertFailEP4char(ptr)

declare i32 @printf(ptr, ...)

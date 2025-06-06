
%S = type <{ i8, i16, i8 }>

@0 = private unnamed_addr constant [43 x i8] c"Assertion failed at packed-struct.cx:11:5\0A\00", align 1

define i32 @main() {
  %s = alloca %S, align 8
  %a = getelementptr inbounds %S, ptr %s, i32 0, i32 0
  store i8 -85, ptr %a, align 1
  %b = getelementptr inbounds %S, ptr %s, i32 0, i32 1
  store i16 -12817, ptr %b, align 2
  %c = getelementptr inbounds %S, ptr %s, i32 0, i32 2
  store i8 0, ptr %c, align 1
  %1 = icmp eq i64 ptrtoint (ptr getelementptr (%S, ptr null, i32 1) to i64), 4
  %assert.condition = icmp eq i1 %1, false
  br i1 %assert.condition, label %assert.fail, label %assert.success

assert.fail:                                      ; preds = %0
  call void @_EN3std10assertFailEP4char(ptr @0)
  unreachable

assert.success:                                   ; preds = %0
  ret i32 0
}

declare void @_EN3std10assertFailEP4char(ptr)

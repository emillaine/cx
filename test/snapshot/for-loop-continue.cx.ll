
%StringIterator = type { ptr, ptr }
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [4 x i8] c"abc\00", align 1

define i32 @main() {
  %__iterator = alloca %StringIterator, align 8
  %__str = alloca %string, align 8
  %ch = alloca i8, align 1
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 3)
  %1 = call %StringIterator @_EN3std6string8iteratorE(ptr %__str)
  store %StringIterator %1, ptr %__iterator, align 8
  br label %loop.condition

loop.condition:                                   ; preds = %loop.increment, %0
  %2 = call i1 @_EN3std14StringIterator8hasValueE(ptr %__iterator)
  br i1 %2, label %loop.body, label %loop.end

loop.body:                                        ; preds = %loop.condition
  %3 = call i8 @_EN3std14StringIterator5valueE(ptr %__iterator)
  store i8 %3, ptr %ch, align 1
  %ch.load = load i8, ptr %ch, align 1
  %4 = icmp eq i8 %ch.load, 98
  br i1 %4, label %if.then, label %if.else

loop.increment:                                   ; preds = %if.end, %if.then
  call void @_EN3std14StringIterator9incrementE(ptr %__iterator)
  br label %loop.condition

loop.end:                                         ; preds = %loop.condition
  ret i32 0

if.then:                                          ; preds = %loop.body
  br label %loop.increment

if.else:                                          ; preds = %loop.body
  br label %if.end

if.end:                                           ; preds = %if.else
  br label %loop.increment
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

declare %StringIterator @_EN3std6string8iteratorE(ptr)

declare i1 @_EN3std14StringIterator8hasValueE(ptr)

declare i8 @_EN3std14StringIterator5valueE(ptr)

declare void @_EN3std14StringIterator9incrementE(ptr)

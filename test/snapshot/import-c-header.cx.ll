
%Foo = type { i32, ptr }

@0 = private unnamed_addr constant [8 x i8] c"foo bar\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"foo\00", align 1

define i32 @main() {
  %f = alloca %Foo, align 8
  %bar = alloca i32, align 4
  %1 = call i32 @puts(ptr @0)
  %bar1 = getelementptr inbounds %Foo, ptr %f, i32 0, i32 0
  store i32 47, ptr %bar1, align 4
  %baz = getelementptr inbounds %Foo, ptr %f, i32 0, i32 1
  store ptr @1, ptr %baz, align 8
  %2 = call i32 @getBar(ptr %f)
  store i32 %2, ptr %bar, align 4
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @getBar(ptr)

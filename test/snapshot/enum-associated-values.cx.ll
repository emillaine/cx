
%E = type { i32, { [8 x i8] } }

define i32 @main() {
  %e = alloca %E, align 8
  %enum = alloca %E, align 8
  %enum1 = alloca %E, align 8
  %enum2 = alloca %E, align 8
  %enum3 = alloca %E, align 8
  %tag = getelementptr inbounds %E, ptr %enum, i32 0, i32 0
  store i32 0, ptr %tag, align 4
  %enum.load = load %E, ptr %enum, align 4
  store %E %enum.load, ptr %e, align 4
  %tag4 = getelementptr inbounds %E, ptr %enum1, i32 0, i32 0
  store i32 1, ptr %tag4, align 4
  %associatedValue = getelementptr inbounds %E, ptr %enum1, i32 0, i32 1
  store { i1, i32 } { i1 false, i32 42 }, ptr %associatedValue, align 4
  %enum.load5 = load %E, ptr %enum1, align 4
  store %E %enum.load5, ptr %e, align 4
  %tag6 = getelementptr inbounds %E, ptr %enum2, i32 0, i32 0
  store i32 2, ptr %tag6, align 4
  %associatedValue7 = getelementptr inbounds %E, ptr %enum2, i32 0, i32 1
  store { i32 } { i32 43 }, ptr %associatedValue7, align 4
  %enum.load8 = load %E, ptr %enum2, align 4
  store %E %enum.load8, ptr %e, align 4
  %e.tag = getelementptr inbounds %E, ptr %e, i32 0, i32 0
  %e.tag.load = load i32, ptr %e.tag, align 4
  switch i32 %e.tag.load, label %switch.default [
    i32 0, label %switch.case.0
    i32 1, label %switch.case.1
    i32 2, label %switch.case.2
  ]

switch.case.0:                                    ; preds = %0
  br label %switch.end

switch.case.1:                                    ; preds = %0
  %1 = getelementptr inbounds %E, ptr %e, i32 0, i32 1
  %tag9 = getelementptr inbounds %E, ptr %enum3, i32 0, i32 0
  store i32 2, ptr %tag9, align 4
  %i = getelementptr inbounds { i1, i32 }, ptr %1, i32 0, i32 1
  %i.load = load i32, ptr %i, align 4
  %2 = insertvalue { i32 } undef, i32 %i.load, 0
  %associatedValue10 = getelementptr inbounds %E, ptr %enum3, i32 0, i32 1
  store { i32 } %2, ptr %associatedValue10, align 4
  %enum.load11 = load %E, ptr %enum3, align 4
  store %E %enum.load11, ptr %e, align 4
  %i12 = getelementptr inbounds { i1, i32 }, ptr %1, i32 0, i32 1
  %i.load13 = load i32, ptr %i12, align 4
  ret i32 %i.load13

switch.case.2:                                    ; preds = %0
  %3 = getelementptr inbounds %E, ptr %e, i32 0, i32 1
  %j = getelementptr inbounds { i32 }, ptr %3, i32 0, i32 0
  %j.load = load i32, ptr %j, align 4
  ret i32 %j.load

switch.default:                                   ; preds = %0
  br label %switch.end

switch.end:                                       ; preds = %switch.default, %switch.case.0
  %e.tag14 = getelementptr inbounds %E, ptr %e, i32 0, i32 0
  %e.tag.load15 = load i32, ptr %e.tag14, align 4
  %4 = icmp eq i32 %e.tag.load15, 0
  %e.tag16 = getelementptr inbounds %E, ptr %e, i32 0, i32 0
  %e.tag.load17 = load i32, ptr %e.tag16, align 4
  %5 = icmp eq i32 %e.tag.load17, 1
  ret i32 0
}

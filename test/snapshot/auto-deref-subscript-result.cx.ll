
%"S<int>" = type { i32 }

define void @_EN4main1fEP1SI3intE(ptr %s) {
  %1 = call ptr @_EN4main1SI3intEixE3int(ptr %s, i32 0)
  %.load = load i32, ptr %1, align 4
  %2 = add i32 %.load, 1
  store i32 %2, ptr %1, align 4
  ret void
}

define ptr @_EN4main1SI3intEixE3int(ptr %this, i32 %i) {
  %t = getelementptr inbounds %"S<int>", ptr %this, i32 0, i32 0
  ret ptr %t
}

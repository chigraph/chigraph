
!0 = !{!"Add two numbers\00", !"2\00", !"a\00", !"b\00", !"out\00"}
!chig-std__add = !{!0}

define i32 @std__add(i32 %a, i32 %b) {
	%tmp = add i32 %a, %b
	
	ret i32 %tmp
}

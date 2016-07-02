
!0 = !{!"Add two numbers", i32 2, !"a", !"b", !"out"}
!chig-std__add = !{!0}

define i32 @std__add(i32 %a, i32 %b) {
	%tmp = add i32 %a, %b
	
	ret i32 %tmp
}

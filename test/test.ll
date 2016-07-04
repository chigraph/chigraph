
!0 = !{!"Add two numbers", !"1", !"2", !"a", !"b", !"out"}
!chig-std__add = !{!0}

define i32 @std__add(i32 %a, i32 %b, i32* %ret) {
	%tmp = add i32 %a, %b
	store i32 %tmp,  i32* %ret
	
	ret i32 0
}

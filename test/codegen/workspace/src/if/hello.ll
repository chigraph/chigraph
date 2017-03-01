; ModuleID = 'if/main'
source_filename = "if/main"
target triple = "x86_64-unknown-linux-gnu"

@0 = private unnamed_addr constant [5 x i8] c"true\00"
@1 = private unnamed_addr constant [6 x i8] c"false\00"

define i32 @main(i32 %inputexec_id) {
alloc:
  call void @llvm.dbg.declare(metadata i32 %inputexec_id, metadata !5, metadata !10), !dbg !11
  %true__0 = alloca i1
  call void @llvm.dbg.declare(metadata i1* %true__0, metadata !12, metadata !10), !dbg !11
  %strtrue__0 = alloca i8*
  call void @llvm.dbg.declare(metadata i8** %strtrue__0, metadata !14, metadata !10), !dbg !11
  %putstrue__0 = alloca i32
  call void @llvm.dbg.declare(metadata i32* %putstrue__0, metadata !17, metadata !10), !dbg !11
  %0 = alloca i8*
  call void @llvm.dbg.declare(metadata i8** %0, metadata !18, metadata !10), !dbg !11
  %1 = alloca i32
  call void @llvm.dbg.declare(metadata i32* %1, metadata !19, metadata !10), !dbg !11
  br label %entry

entry:                                            ; preds = %alloc
  br label %entry_code

entry_code:                                       ; preds = %entry
  switch i32 %inputexec_id, label %node_if [
    i32 0, label %node_if
  ], !dbg !11

node_if:                                          ; preds = %entry_code, %entry_code
  br label %true____if

if_code:                                          ; preds = %true_code
  %2 = load i1, i1* %true__0
  br i1 %2, label %node_putstrue, label %node_putsfalse, !dbg !20

true____if:                                       ; preds = %node_if
  br label %true_code

true_code:                                        ; preds = %true____if
  store i1 true, i1* %true__0, !dbg !21
  br label %if_code, !dbg !21

node_putstrue:                                    ; preds = %if_code
  br label %strtrue____putstrue

node_putsfalse:                                   ; preds = %if_code
  br label %9

putstrue_code:                                    ; preds = %strtrue_code
  %3 = load i8*, i8** %strtrue__0
  %4 = call i32 @doputs1(i8* %3), !dbg !22
  store i32 %4, i32* %putstrue__0, !dbg !22
  br label %node_exit, !dbg !22

strtrue____putstrue:                              ; preds = %node_putstrue
  br label %strtrue_code

strtrue_code:                                     ; preds = %strtrue____putstrue
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0), i8** %strtrue__0, !dbg !23
  br label %putstrue_code, !dbg !23

node_exit:                                        ; preds = %6, %putstrue_code
  br label %5

; <label>:5:                                      ; preds = %node_exit
  ret i32 0, !dbg !24

; <label>:6:                                      ; preds = %10
  %7 = load i8*, i8** %0
  %8 = call i32 @doputs(i8* %7), !dbg !25
  store i32 %8, i32* %1, !dbg !25
  br label %node_exit, !dbg !25

; <label>:9:                                      ; preds = %node_putsfalse
  br label %10

; <label>:10:                                     ; preds = %9
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0), i8** %0, !dbg !26
  br label %6, !dbg !26
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

; Function Attrs: nounwind uwtable
define i32 @doputs1(i8*) #1 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i8*, i8** %2, align 8
  %4 = call i32 @puts(i8* %3)
  ret i32 %4
}

declare i32 @puts(i8*) #2

; Function Attrs: nounwind uwtable
define i32 @doputs(i8*) #1 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i8*, i8** %2, align 8
  %4 = call i32 @puts(i8* %3)
  ret i32 %4
}

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.dbg.cu = !{!1}
!llvm.ident = !{!4, !4}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !DICompileUnit(language: DW_LANG_C, file: !2, producer: "Chigraph Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !3)
!2 = !DIFile(filename: "main.chigmod", directory: "/home/russellg/projects/chigraph/test/codegen/workspace/src/if")
!3 = !{}
!4 = !{!"clang version 3.9.1 (tags/RELEASE_391/final)"}
!5 = !DILocalVariable(name: "inputexec_id", arg: 1, scope: !6, file: !2, type: !9)
!6 = distinct !DISubprogram(name: "if/main:main", linkageName: "main", scope: !2, file: !2, line: 1, type: !7, isLocal: false, isDefinition: true, isOptimized: false, unit: !1, variables: !3)
!7 = !DISubroutineType(types: !8)
!8 = !{!9, !9}
!9 = !DIBasicType(name: "lang:i32", size: 32, align: 32, encoding: DW_ATE_signed)
!10 = !DIExpression()
!11 = !DILocation(line: 1, column: 1, scope: !6)
!12 = !DILocalVariable(name: "true__0", scope: !6, file: !2, line: 1, type: !13)
!13 = !DIBasicType(name: "lang:i1", size: 8, align: 8, encoding: DW_ATE_boolean)
!14 = !DILocalVariable(name: "strtrue__0", scope: !6, file: !2, line: 1, type: !15)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !16, size: 64, align: 64)
!16 = !DIBasicType(name: "lang:i8", size: 8, align: 8, encoding: DW_ATE_unsigned_char)
!17 = !DILocalVariable(name: "putstrue__0", scope: !6, file: !2, line: 1, type: !9)
!18 = !DILocalVariable(name: "strfalse__0", scope: !6, file: !2, line: 1, type: !15)
!19 = !DILocalVariable(name: "putsfalse__0", scope: !6, file: !2, line: 1, type: !9)
!20 = !DILocation(line: 3, column: 1, scope: !6)
!21 = !DILocation(line: 8, column: 1, scope: !6)
!22 = !DILocation(line: 5, column: 1, scope: !6)
!23 = !DILocation(line: 7, column: 1, scope: !6)
!24 = !DILocation(line: 2, column: 1, scope: !6)
!25 = !DILocation(line: 4, column: 1, scope: !6)
!26 = !DILocation(line: 6, column: 1, scope: !6)

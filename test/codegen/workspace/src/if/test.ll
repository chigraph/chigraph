; ModuleID = 'if/main'
source_filename = "if/main"
target triple = "x86_64-unknown-linux-gnu"

@0 = private unnamed_addr constant [5 x i8] c"true\00"
@1 = private unnamed_addr constant [6 x i8] c"false\00"

define i32 @main(i32 %inputexec_id) !dbg !5 {
alloc:
  call void @llvm.dbg.declare(metadata i32 %inputexec_id, metadata !9, metadata !10), !dbg !11
  %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7__0 = alloca i1
  call void @llvm.dbg.declare(metadata i1* %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7__0, metadata !12, metadata !10), !dbg !14
  %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44__0 = alloca i8*
  call void @llvm.dbg.declare(metadata i8** %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44__0, metadata !15, metadata !10), !dbg !14
  %"2b0c944e-7556-4b1d-b11b-fce5c922c08a__0" = alloca i32
  call void @llvm.dbg.declare(metadata i32* %"2b0c944e-7556-4b1d-b11b-fce5c922c08a__0", metadata !18, metadata !10), !dbg !14
  %"7bd619d5-0bf6-419f-9182-39c8a463c9fa__0" = alloca i8*
  call void @llvm.dbg.declare(metadata i8** %"7bd619d5-0bf6-419f-9182-39c8a463c9fa__0", metadata !19, metadata !10), !dbg !14
  %"425a5fbe-f47a-46b4-a584-02bd5d9a630e__0" = alloca i32
  call void @llvm.dbg.declare(metadata i32* %"425a5fbe-f47a-46b4-a584-02bd5d9a630e__0", metadata !20, metadata !10), !dbg !14
  br label %a8bf0c2e-04b7-45e0-b674-d17e221d8052

a8bf0c2e-04b7-45e0-b674-d17e221d8052:             ; preds = %alloc
  br label %a8bf0c2e-04b7-45e0-b674-d17e221d8052_code

a8bf0c2e-04b7-45e0-b674-d17e221d8052_code:        ; preds = %a8bf0c2e-04b7-45e0-b674-d17e221d8052
  switch i32 %inputexec_id, label %node_d74771c0-a287-4c64-a831-d9cb6cf42a9f [
    i32 0, label %node_d74771c0-a287-4c64-a831-d9cb6cf42a9f
  ], !dbg !11

node_d74771c0-a287-4c64-a831-d9cb6cf42a9f:        ; preds = %a8bf0c2e-04b7-45e0-b674-d17e221d8052_code, %a8bf0c2e-04b7-45e0-b674-d17e221d8052_code
  br label %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7____d74771c0-a287-4c64-a831-d9cb6cf42a9f

d74771c0-a287-4c64-a831-d9cb6cf42a9f_code:        ; preds = %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7_code
  %0 = load i1, i1* %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7__0
  br i1 %0, label %node_2b0c944e-7556-4b1d-b11b-fce5c922c08a, label %node_425a5fbe-f47a-46b4-a584-02bd5d9a630e, !dbg !21

d1fd3c93-85f6-43ea-90bb-437fee5cf9e7____d74771c0-a287-4c64-a831-d9cb6cf42a9f: ; preds = %node_d74771c0-a287-4c64-a831-d9cb6cf42a9f
  br label %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7_code

d1fd3c93-85f6-43ea-90bb-437fee5cf9e7_code:        ; preds = %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7____d74771c0-a287-4c64-a831-d9cb6cf42a9f
  store i1 true, i1* %d1fd3c93-85f6-43ea-90bb-437fee5cf9e7__0, !dbg !22
  br label %d74771c0-a287-4c64-a831-d9cb6cf42a9f_code, !dbg !22

node_2b0c944e-7556-4b1d-b11b-fce5c922c08a:        ; preds = %d74771c0-a287-4c64-a831-d9cb6cf42a9f_code
  br label %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44____2b0c944e-7556-4b1d-b11b-fce5c922c08a

node_425a5fbe-f47a-46b4-a584-02bd5d9a630e:        ; preds = %d74771c0-a287-4c64-a831-d9cb6cf42a9f_code
  br label %"7bd619d5-0bf6-419f-9182-39c8a463c9fa____425a5fbe-f47a-46b4-a584-02bd5d9a630e"

"2b0c944e-7556-4b1d-b11b-fce5c922c08a_code":      ; preds = %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44_code
  %1 = load i8*, i8** %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44__0
  store i32 %2, i32* %"2b0c944e-7556-4b1d-b11b-fce5c922c08a__0", !dbg !14
  br label %node_4a38557a-2555-4f76-bda7-aa0395d1f6a3, !dbg !14

ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44____2b0c944e-7556-4b1d-b11b-fce5c922c08a: ; preds = %node_2b0c944e-7556-4b1d-b11b-fce5c922c08a
  br label %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44_code

ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44_code:        ; preds = %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44____2b0c944e-7556-4b1d-b11b-fce5c922c08a
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0), i8** %ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44__0, !dbg !23
  br label %"2b0c944e-7556-4b1d-b11b-fce5c922c08a_code", !dbg !23

node_4a38557a-2555-4f76-bda7-aa0395d1f6a3:        ; preds = %"425a5fbe-f47a-46b4-a584-02bd5d9a630e_code", %"2b0c944e-7556-4b1d-b11b-fce5c922c08a_code"
  br label %"4a38557a-2555-4f76-bda7-aa0395d1f6a3_code"

"4a38557a-2555-4f76-bda7-aa0395d1f6a3_code":      ; preds = %node_4a38557a-2555-4f76-bda7-aa0395d1f6a3
  ret i32 0, !dbg !24

"425a5fbe-f47a-46b4-a584-02bd5d9a630e_code":      ; preds = %"7bd619d5-0bf6-419f-9182-39c8a463c9fa_code"
  %3 = load i8*, i8** %"7bd619d5-0bf6-419f-9182-39c8a463c9fa__0"
  %4 = call i32 @doputs(i8* %3), !dbg !25
  store i32 %4, i32* %"425a5fbe-f47a-46b4-a584-02bd5d9a630e__0", !dbg !25
  br label %node_4a38557a-2555-4f76-bda7-aa0395d1f6a3, !dbg !25

"7bd619d5-0bf6-419f-9182-39c8a463c9fa____425a5fbe-f47a-46b4-a584-02bd5d9a630e": ; preds = %node_425a5fbe-f47a-46b4-a584-02bd5d9a630e
  br label %"7bd619d5-0bf6-419f-9182-39c8a463c9fa_code"

"7bd619d5-0bf6-419f-9182-39c8a463c9fa_code":      ; preds = %"7bd619d5-0bf6-419f-9182-39c8a463c9fa____425a5fbe-f47a-46b4-a584-02bd5d9a630e"
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0), i8** %"7bd619d5-0bf6-419f-9182-39c8a463c9fa__0", !dbg !26
  br label %"425a5fbe-f47a-46b4-a584-02bd5d9a630e_code", !dbg !26
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

; Function Attrs: noinline nounwind uwtable
define i32 @doputs1(i8* %arg) #1 {
entry:
  %arg.addr = alloca i8*, align 8
  store i8* %arg, i8** %arg.addr, align 8
  %0 = load i8*, i8** %arg.addr, align 8
  %call = call i32 @puts(i8* %0)
  ret i32 %call
}

declare i32 @puts(i8*) #2

; Function Attrs: noinline nounwind uwtable
define i32 @doputs(i8* %arg) #1 {
entry:
  %arg.addr = alloca i8*, align 8
  store i8* %arg, i8** %arg.addr, align 8
  %0 = load i8*, i8** %arg.addr, align 8
  %call = call i32 @puts(i8* %0)
  ret i32 %call
}

attributes #0 = { nounwind readnone }
attributes #1 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.dbg.cu = !{!1}
!llvm.ident = !{!4, !4}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !DICompileUnit(language: DW_LANG_C, file: !2, producer: "Chigraph Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !3)
!2 = !DIFile(filename: "main.chigmod", directory: "/home/russellg/projects/chigraph/test/codegen/workspace/src/if")
!3 = !{}
!4 = !{!"clang version 4.0.0 (https://github.com/llvm-mirror/clang 7d2f60d33dcc26e06ffbac2d95c7c3bab044fadf) (https://github.com/llvm-mirror/llvm 96c0db119bcb6b9d6a4ed3eb95d0b992c086aef0)"}
!5 = distinct !DISubprogram(name: "if/main:main", linkageName: "main", scope: !2, file: !2, line: 5, type: !6, isLocal: false, isDefinition: true, isOptimized: false, unit: !1, variables: !3)
!6 = !DISubroutineType(types: !7)
!7 = !{!8, !8}
!8 = !DIBasicType(name: "lang:i32", size: 32, align: 32, encoding: DW_ATE_signed)
!9 = !DILocalVariable(name: "inputexec_id", arg: 1, scope: !5, file: !2, line: 5, type: !8)
!10 = !DIExpression()
!11 = !DILocation(line: 5, column: 1, scope: !5)
!12 = !DILocalVariable(name: "d1fd3c93-85f6-43ea-90bb-437fee5cf9e7__0", scope: !5, file: !2, line: 1, type: !13)
!13 = !DIBasicType(name: "lang:i1", size: 8, align: 8, encoding: DW_ATE_boolean)
!14 = !DILocation(line: 1, column: 1, scope: !5)
!15 = !DILocalVariable(name: "ddc2f8f6-5c48-4b49-8d40-d68ad3abfe44__0", scope: !5, file: !2, line: 1, type: !16)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64, align: 64)
!17 = !DIBasicType(name: "lang:i8", size: 8, align: 8, encoding: DW_ATE_unsigned_char)
!18 = !DILocalVariable(name: "2b0c944e-7556-4b1d-b11b-fce5c922c08a__0", scope: !5, file: !2, line: 1, type: !8)
!19 = !DILocalVariable(name: "7bd619d5-0bf6-419f-9182-39c8a463c9fa__0", scope: !5, file: !2, line: 1, type: !16)
!20 = !DILocalVariable(name: "425a5fbe-f47a-46b4-a584-02bd5d9a630e__0", scope: !5, file: !2, line: 1, type: !8)
!21 = !DILocation(line: 7, column: 1, scope: !5)
!22 = !DILocation(line: 6, column: 1, scope: !5)
!23 = !DILocation(line: 8, column: 1, scope: !5)
!24 = !DILocation(line: 3, column: 1, scope: !5)
!25 = !DILocation(line: 2, column: 1, scope: !5)
!26 = !DILocation(line: 4, column: 1, scope: !5)

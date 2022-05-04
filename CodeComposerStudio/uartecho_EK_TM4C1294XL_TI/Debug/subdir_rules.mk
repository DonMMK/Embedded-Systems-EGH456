################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI" --include_path="/Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI" --include_path="/Users/don/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/Users/don/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-157103057:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-157103057-inproc

build-157103057-inproc: ../uartecho.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/Applications/ti/ccs1110/xdctools_3_32_00_06_core/xs" --xdcpath="/Users/don/ti/tirtos_tivac_2_16_00_08/packages;/Users/don/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/Users/don/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/Users/don/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/Users/don/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/Users/don/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C1294NCPDT -r release -c "/Applications/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"/Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI\" --include_path=\"/Users/don/GitHub/Embedded-Systems-EGH456/CodeComposerStudio/uartecho_EK_TM4C1294XL_TI\" --include_path=\"/Users/don/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"/Users/don/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix\" --include_path=\"/Applications/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include\" --define=ccs=\"ccs\" --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-157103057 ../uartecho.cfg
configPkg/compiler.opt: build-157103057
configPkg/: build-157103057



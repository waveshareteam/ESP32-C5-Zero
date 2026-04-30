# 11_mem_safe（ESP32-C5）操作流程与现象说明

## 1. 工程目的

本工程用于演示 ESP32-C5 在内存与安全相关方向的若干能力，包含：

- 安全状态打印（编译期配置、运行期状态）
- 当前特权状态打印（RISC-V mstatus）
- 硬件密码能力验证（SHA256）
- APM/TEE 能力探测输出
- 可选的 PMP 执行权限故障注入（默认关闭）

主入口为 `main/main.c`，启动后调用 `security_demo_run()`，随后进入常驻循环。

## 2. 目录说明

- `main/main.c`：应用入口
- `components/security_demo/security_demo.c`：安全演示主流程与各子功能实现
- `components/security_demo/include/security_demo.h`：对外接口与故障注入开关

## 3. 默认开关（当前工程）

`components/security_demo/include/security_demo.h` 默认值：

- `SECURITY_DEMO_ENABLE_FAULT_INJECT = 0`

说明：

- 安全状态打印、特权状态打印、SHA256 测试、TEE/APM 探测为固定执行流程
- 仅故障注入路径保留宏开关，避免低价值条件编译使代码变碎

## 4. 操作流程

以下流程基于 ESP-IDF 常规命令：

1. 进入工程目录：`11_mem_safe`
2. 配置目标芯片（首次或目标变化时）：`idf.py set-target esp32c5`
3. 编译：`idf.py build`
4. 烧录：`idf.py -p <串口号> flash`
5. 监视串口日志：`idf.py -p <串口号> monitor`

说明：

- VS Code 配置中已有串口示例：`COM53`（见 `.vscode/settings.json`）
- 若串口不同，请替换为实际端口

## 5. 默认运行现象（故障注入关闭）

上电后通常会看到类似前缀日志：

- `security_demo: compile_time: ...`
- `security_demo: flash_size=...`
- `security_demo: flash_encryption_enabled=...`
- `security_demo: secure_boot_enabled=...`
- `security_demo: riscv_csr_mstatus=...`
- `security_demo: hw_crypto_test: SHA256_ret=0`
- `security_demo: hw_crypto_test: SHA256_result=xxxxxxxx...`
- `security_demo: tee_check: ...`
- `security_demo: apm_test: ...`

现象解释：

- `SHA256_ret=0` 代表摘要计算成功
- `secure_boot_enabled`、`flash_encryption_enabled` 会按当前 `sdkconfig` 输出实际状态
- 默认不会触发异常，系统保持运行

## 6. 故障注入流程（可选）

若要验证 PMP 执行权限拦截：

1. 打开 `components/security_demo/include/security_demo.h`
2. 将 `SECURITY_DEMO_ENABLE_FAULT_INJECT` 从 `0` 改为 `1`
3. 重新执行编译、烧录、监视

预期现象：

- 启动后先完成前面若干打印
- 延时约 1 秒后进入故障注入路径
- 尝试执行普通内存中的指令，触发执行权限异常（或类似 fault/panic 输出）
- 设备可能复位或停在异常处理流程（与具体配置有关）

## 7. 常见问题

1) 看不到 `security_demo:` 日志  
检查是否成功烧录，且 monitor 连接的是正确串口。

2) `SHA256_ret` 非 0  
检查编译配置、IDF 环境和芯片运行状态，建议先 clean 后重新 build。

3) 开启 PMP 注入后频繁重启  
这是预期行为之一，说明触发了异常路径；如需恢复稳定运行，将 `SECURITY_DEMO_ENABLE_FAULT_INJECT` 改回 `0`。

## 8. 当前工程安全状态提示

根据现有 `sdkconfig`，当前工程以“功能验证/演示”配置为主。是否启用 Secure Boot、Flash Encryption，取决于你的实际产品需求与量产策略。建议在量产前单独建立“加固配置”进行验证。

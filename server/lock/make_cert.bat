@echo off
SETLOCAL
set PASS=123456987

set OPENSSL_CONF=C:\msys64\usr\ssl\openssl.cnf
REM 设置证书文件名
set "CA_CERT=ca.crt"
set "CA_KEY=ca.key"
set "CA_SUBJECT=/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=RootCA"

set "LOCK_KEY=lock.key"
set "LOCK_CSR=lock.csr"
set "LOCK_CERT=lock.crt"
set "LOCK_SUBJECT=/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=192.168.101.9"

REM 检查ca.crt是否存在
if not exist "%CA_CERT%" (
    echo ca.crt 不存在，生成新的 CA 证书和密钥...
    REM 生成 CA 私钥
    openssl req -newkey rsa:2048 -x509 -new -nodes -sha256 -days 3650 -keyout "%CA_KEY%" -out "%CA_CERT%" -subj "%CA_SUBJECT%" -passout pass:%PASS%
) else (
    echo ca.crt 已存在，跳过 CA 证书生成...
)

REM 删除现有的 lock.key, lock.csr, lock.crt
echo 删除现有的 %LOCK_KEY%, %LOCK_CSR%, %LOCK_CERT%...
del /f "%LOCK_KEY%" "%LOCK_CSR%" "%LOCK_CERT%" 2>nul


REM 生成 CSR
echo 生成新的证书签名请求（CSR）...
openssl req -new -newkey rsa:2048 -keyout "%LOCK_KEY%" -out "%LOCK_CSR%" -subj "%LOCK_SUBJECT%"  -passout pass:%PASS%

REM 使用 CA 签署 CSR，生成服务器证书
echo 使用 CA 签署 CSR，生成服务器证书...
openssl x509 -req -in "%LOCK_CSR%" -CA "%CA_CERT%" -CAkey "%CA_KEY%" -CAcreateserial -out "%LOCK_CERT%" -days 365 -sha256 -passin pass:%PASS%

echo 操作完成。生成的文件有：
echo   CA 证书: %CA_CERT%
echo   CA 私钥: %CA_KEY%
echo   服务器私钥: %LOCK_KEY%
echo   服务器证书: %LOCK_CERT%
endlocal
pause
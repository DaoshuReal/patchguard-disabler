param(
    [string]$DriverPath
)

$ErrorActionPreference = "Stop"

$SignTool = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.28000.0\x64\signtool.exe"

$Subject = "CN=PG Test"

$Existing = Get-AuthenticodeSignature -FilePath $DriverPath

if ($Existing.Status -eq "Valid") {
    if ($Existing.SignerCertificate.Subject -eq $Subject) {
        exit 0
    }
}

$Found = Get-ChildItem -Path "Cert:\CurrentUser\My" | Where-Object { $_.Subject -eq $Subject } | Select-Object -First 1

if ($null -eq $Found) {
    $Found = New-SelfSignedCertificate -Type CodeSigningCert -Subject $Subject -CertStoreLocation "Cert:\CurrentUser\My" -NotAfter (Get-Date).AddYears(10) -KeyLength 2048 -KeyExportPolicy Exportable
}

& $SignTool sign /v /s MY /sha1 $Found.Thumbprint /fd SHA256 $DriverPath

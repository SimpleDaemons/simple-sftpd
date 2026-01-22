# Simple Secure FTP Daemon - Security Diagrams

## Security Architecture

```mermaid
graph TB
    subgraph "Network Security"
        Firewall[Firewall<br/>Port 21/990]
        DDoSProtection[DDoS Protection<br/>Rate Limiting]
    end

    subgraph "Transport Security"
        TLS[TLS/SSL<br/>FTPS Encryption]
        Certificate[Certificate Management<br/>SSL/TLS Certs]
    end

    subgraph "Access Control"
        ACL[Access Control Lists<br/>IP/Network Based]
        UserAuth[User Authentication<br/>Password/Key]
        Chroot[Chroot Jail<br/>Directory Isolation]
    end

    subgraph "File Security"
        Permissions[File Permissions<br/>Read/Write/Execute]
        PathValidation[Path Validation<br/>Directory Traversal Protection]
        Quota[Quota Management<br/>Disk Space Limits]
    end

    Firewall --> TLS
    DDoSProtection --> ACL

    TLS --> Certificate
    Certificate --> UserAuth

    ACL --> UserAuth
    UserAuth --> Chroot

    Chroot --> Permissions
    Permissions --> PathValidation
    PathValidation --> Quota
```

## Security Flow

```mermaid
flowchart TD
    Start([FTP Connection Received]) --> ExtractInfo[Extract Client Info<br/>IP, Port]

    ExtractInfo --> ACLCheck{ACL Check}
    ACLCheck -->|Blocked| LogBlock1[Log Security Event<br/>ACL Blocked]
    ACLCheck -->|Allowed| RateLimitCheck

    RateLimitCheck{Rate Limiting Check}
    RateLimitCheck -->|Exceeded| LogBlock2[Log Security Event<br/>Rate Limited]
    RateLimitCheck -->|Within Limits| TLSCheck

    TLSCheck{TLS Required?}
    TLSCheck -->|Yes & Plain| RejectTLS[Reject - TLS Required]
    TLSCheck -->|TLS or Not Required| AuthCheck

    AuthCheck[Authentication] --> ValidateUser{Validate Username}
    ValidateUser -->|Invalid| LogBlock3[Log Security Event<br/>Invalid User]
    ValidateUser -->|Valid| ValidatePass{Validate Password}

    ValidatePass -->|Invalid| LogBlock4[Log Security Event<br/>Invalid Password]
    ValidatePass -->|Valid| ChrootCheck

    ChrootCheck[Apply Chroot] --> PermissionCheck{Permission Check}
    PermissionCheck -->|Denied| LogBlock5[Log Security Event<br/>Permission Denied]
    PermissionCheck -->|Allowed| ProcessCommand

    ProcessCommand[Process FTP Command] --> PathValidation{Path Validation}
    PathValidation -->|Invalid| LogBlock6[Log Security Event<br/>Path Traversal]
    PathValidation -->|Valid| ExecuteCommand

    ExecuteCommand[Execute Command] --> End([End])

    LogBlock1 --> End
    LogBlock2 --> End
    LogBlock3 --> End
    LogBlock4 --> End
    LogBlock5 --> End
    LogBlock6 --> End
    RejectTLS --> End
```

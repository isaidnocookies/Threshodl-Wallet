CREATE TABLE bills (
    id         INTEGER    PRIMARY KEY AUTOINCREMENT
                          UNIQUE
                          NOT NULL,
    owner      STRING     NOT NULL,
    wallet     INTEGER    CONSTRAINT bill_ownership REFERENCES wallet (id) ON DELETE CASCADE
                                                                           ON UPDATE CASCADE
                          NOT NULL,
    address    STRING     NOT NULL,
    publicKey  STRING     NOT NULL,
    privateKey STRING     NOT NULL,
    currency   STRING (4) NOT NULL,
    amount     STRING     NOT NULL
);

CREATE TABLE wallets (
    id       INTEGER    PRIMARY KEY AUTOINCREMENT
                        UNIQUE
                        NOT NULL,
    type     INTEGER    NOT NULL,
    owner    STRING     NOT NULL,
    currency STRING (4) NOT NULL,
    amount   STRING     NOT NULL
);

CREATE TABLE settings (
    parameter STRING,
    value     STRING
);


CREATE TABLE bill (
    id      INTEGER PRIMARY KEY
                    UNIQUE
                    NOT NULL,
    address STRING  NOT NULL,
    public  STRING  NOT NULL,
    private STRING  NOT NULL,
    amount  STRING  NOT NULL,
    wallet  INTEGER REFERENCES wallet (id) ON DELETE CASCADE
                                           ON UPDATE CASCADE
                                           MATCH SIMPLE
                    NOT NULL
);

CREATE TABLE wallet (
    id       INTEGER    PRIMARY KEY
                        UNIQUE
                        NOT NULL,
    owner    STRING     UNIQUE
                        NOT NULL,
    amount   STRING     NOT NULL,
    currency STRING (3) NOT NULL,
    type     INTEGER    NOT NULL
);

CREATE TABLE settings (
    parameter STRING,
    value     STRING
);


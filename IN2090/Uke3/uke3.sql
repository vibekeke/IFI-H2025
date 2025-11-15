
--    Oppgave 1: CREATE TABLE

--Skriv SQL-setninger som oppretter tabellene i skjemaet. 
--Finn passende datatyper for attributtene. I tillegg ønsker vi at attributtet status i 
--relasjonen Prosjekt kun skal kunne inneholde verdiene 'planlagt', 'aktiv', eller 'ferdig'.


CREATE TABLE kunde (
    kundenummer int,
    kundenavn text NOT NULL,
    kundeadresse text,
    postnr int,
    poststed text,
    CONSTRAINT knr_pk PRIMARY KEY(kundenummer)
);

CREATE TABLE prosjekt(
    prosjektnr int UNIQUE,
    prosjektleder text REFERENCES ansatt(ansattnr),
    prosjektnavn text NOT NULL,
    kundenummer int REFERENCES kunde(kundenummer),
    p_status text CHECK(p_status = 'planlagt' OR p_status = 'aktiv' OR p_status = 'ferdig')
);

CREATE TABLE ansatt(
    ansattnr int PRIMARY KEY,
    navn text NOT NULL,
    fødselsdato date,
    ansattDato date, 
);

CREATE TABLE ansattDeltarIProsjekt(
    ansattnr int REFERENCES ansatt(ansattnr), 
    prosjektnr int NOT NULL REFERENCES prosjekt(prosjektnummer)
    CONSTRAINT deltar_pk PRIMARY KEY (ansattnr, prosjektnr)
);


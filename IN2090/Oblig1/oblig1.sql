
-- Oppgave 1 --

SELECT count(*) FROM materie;
--gir 403.


-- Oppgave 2 --
-- a)
SELECT p.navn
FROM planet AS p 
INNER JOIN stjerne AS s ON p.stjerne = s.navn
WHERE s.navn = 'Proxima Centauri';

-- b)
SELECT p.oppdaget
FROM planet p INNER JOIN stjerne s ON p.stjerne = s.navn
WHERE s.navn = 'TRAPPIST-1' OR s.navn = 'Kepler-154';

-- c)
SELECT count(*)
FROM planet
WHERE masse IS NULL;

--d) 
SELECT navn, masse
FROM planet
WHERE oppdaget = 2020
    AND masse > (SELECT avg(masse) FROM planet);

--e)
SELECT max(oppdaget) - min(oppdaget)
FROM planet;


-- Oppgave 3 --
-- a)
SELECT p.navn
FROM planet p 
    INNER JOIN materie m ON p.navn = m.planet
WHERE p.masse >= 3 AND p.masse <= 10 AND m.molekyl = 'H2O';

-- b)
SELECT p.navn
FROM planet AS p
INNER JOIN stjerne s ON p.stjerne = s.navn
INNER JOIN materie AS m ON p.navn = m.planet
WHERE s.avstand <= (s.masse * 12)
AND m.molekyl LIKE '%H%';


-- c)
SELECT p.navn
FROM planet p
INNER JOIN planet p2 ON p.stjerne = p2.stjerne AND p.navn != p2.navn
INNER JOIN stjerne s ON p.stjerne = s.navn
WHERE p.masse > 10 AND p2.masse > 10
    AND s.avstand < 50;




-- Oppgave 4 --
--Problemet med å bruke NATURAL JOIN her er at den slår sammen kolonnene navn i tabellen stjerne og navn i tabellen planet.
--Resultatet er at du kun får rader der stjerne.navn = planet.navn, noe som sjeldent vil være tilfellet (om ikke aldri).
--Uansett blir det feil svar.


-- Oppgave 5 --
-- a) 
INSERT INTO stjerne
VALUES ('Sola', 0, 1);

-- b)
INSERT INTO planet
VALUES ('Jorda', 0.003146, NULL, 'Sola');



-- Oppgave 6 --
CREATE TABLE observasjon (
    observasjons_id int PRIMARY KEY,
    tidspunkt timestamp,
    planet text REFERENCES planet(navn),
    kommentar text
);
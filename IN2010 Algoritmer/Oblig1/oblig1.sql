
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
-- b)
-- c)




-- Oppgave 4 --
--Tekstoppgave--



-- Oppgave 5 --
-- a) 
-- b)



-- Oppgave 6 --
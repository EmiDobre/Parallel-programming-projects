# tema1

Tema 1 APD - Dobre Emilia Iliana 333CB

-----Etapele codului si nevoia de paralelizare:-----

1) Variabile importante: 
    - image:        se citeste si initializeaza imaginea de transformat, 
    - contour_map:  se aloca imaginea de "contururi"
    - scaled_image: se aloca imaginea ce urmeaza a fi scalata

Aceste variabile vor fi parte din memoria partajata a threadurilor 
astfel este nevoie sa le retin intr-o structura numita Arguments, structura ce va fi argumentul functiei de threaduri si mai mult este nevoie ca acestea sa fie alocate o singura data inainte de inceperea functiilor pentru a putea sa functioneze corect.

2) Paralelizare:
(MAIN):
    - initializez structura cu variabilele necesare
    - se intrerupe threadul main si se incepe executarea in paralel a functiei de thread de threaduri

(THREAD_FUNCTION):
    - apeleaza functiile din codul secvential care au fost paralelizate
    - am ales sa paralelizez doar functia de intializare de contour_map si cea de rescalare a imaginii intrucat nu au existat probleme de sincronizare intre executarile lor paralele nefiind nevoie de bariere care sa incetineasca codul

(INIT_CONTOUR_MAP):
    - functia mapeaza fiecare configuratie la imaginea corespunzatoare
    - am impartit echitabil intre threaduri cele 16 configuratii care urmeaza sa fie asociate cu o imagine 

(RESCALE_IMAGE):
    - imaginea intiala se scaleaza la valorile date doar daca este nevoie 
    - imaginea scalata va fi construita in paralel de threadurile care vor scala fiecare un numar de linii din matricea vectorizat de pixeli 
    - am impartit indecsii "liniilor" pe care un thread le scaleaza 
    - astfel nu exista probleme de race condition, sincronizare, paralelizarea functiei duce la timpi foarte mici dat fiind valorile foarte mari a matricei de pixeli

(Inapoi in MAIN):
    - executia in paralel s-a oprit si se continua secvential
    algortmul ramas

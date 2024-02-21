Tema2 - Dobre Emilia Iliana 333 CB

CLASA DISPATCHER:
-    
 - Scopul clasei este sa organizeze trimiterea taskurilor care se primesc de la TaskGenerator
 - In functie de algoritmul ales:

   1) Round Robin: 
   
            - taskul primit se duce in hostul ce are numarul dat de
            - ultimul id + 1 % numarul hostilor

   2) Size Interval Task Asignment:
           
            - sunt doar 3 hosti disponibili 
            - se verifica tipul taskului primit
   3) Shortest Queue:
            
            - taskul se duce la hostul care are coada de dimensiune minima
            - verific coada tuturor hostilor si gasesc id ul hostului de coada minima
   
   4) Least Work Left:
         
            - se gaseste id ul hostului la fel ca la 3 care de data aceasta
            - trebuie sa aiba cea mai putina "munca" de facut 


CLASA HOST:
-  
- Host primeste taskuri la diferite momente de timp 
- Organizeaza momentul de executie al taskurilor in functie de 
  momentul la care ajung taskurile si de prioritatea lor

  I. Structuri
                     
         -     coada prioritati - suprascriere metodei de compareTo a clasei Task
         -     => astfel cand se primesc taskuri in acelasi timp ele sunt organizate 
         -     in functie de prioritate

  II. Metode:
  
     1) addTask
             
            - se primeste taskul la un moment dat
            - se adauga in coada 
     2) run
            
            -  hostul ruleaza taskuri din coada pana cand se da shutdown
            -  din coada taskurile se executa pe rand ele fiind deja aranajte 
               dupa prioritate
            -  executia inseamna ca taskul running consuma timp pe procesor
               pana cand nu mai are timp ramas
            -  abia atunci se poate apela metoda de finish din clasa Task
            
            -  in timpul executiei unui task preemptibil, aparitia unui nou task 
               in coada de prioritati care are prioritate mai mare duce 
               la oprirea taskului curent
  
            - running acum este acest nou task pana cand se termina sau apare
              iar situatia anterioara
            
            - taskul care a fost oprit trebuie bagat inapoi in coada pentru
              a-si relua executia cand ii vine randul
   
     3) getWorkLeft:
            
             - munca ramasa reprezinta suma timpului ramas al tuturor taskurilor din coada
               dar si a taskului running
  
     4) getQueueSize:
              
            - dimensiunea cozii este data si de taskul care este acum running
 
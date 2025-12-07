import json
import os
import sys

def dodaj_regule_klasyfikacji():
    json_path = "classify.json" 

    try:
        if not os.path.exists(json_path):
            print(f"\n❌ Błąd: Nie znaleziono pliku pod ścieżką: {json_path}")
            print("Upewnij się, że plik JSON istnieje w katalogu, z którego uruchamiasz skrypt.")
            sys.exit(1)
        
        nowa_regula = input("Podaj nową regułę do dodania (np. 'Classification = 38 WHERE Linearity > 0.9'): ").strip()
        if not nowa_regula:
            print("\n⚠️ Ostrzeżenie: Nie podano żadnej reguły. Zakończono bez zmian.")
            sys.exit(0)
        
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)

        assign_value_list = None
        
        if "pipeline" in data and isinstance(data["pipeline"], list):
            for stage in data["pipeline"]:
                if isinstance(stage, dict) and stage.get("type") == "filters.assign":
                    if "value" in stage and isinstance(stage["value"], list):
                        assign_value_list = stage["value"]
                        break

        if assign_value_list is None:
            print("\n❌ Błąd: Nie znaleziono etapu 'filters.assign' lub klucz 'value' nie jest listą w tym etapie.")
            sys.exit(1)

        if nowa_regula not in assign_value_list:
            assign_value_list.append(nowa_regula)
            print(f"\n✅ Sukces: Dodano nową regułę do listy 'value' w '{json_path}'.")
        else:
            print("\n⚠️ Ostrzeżenie: Ta reguła już istnieje w liście 'value'. Plik nie został zmieniony.")
            sys.exit(0)
            
        
        with open(json_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4)
        
        print(f"Plik '{json_path}' został pomyślnie zaktualizowany i zapisany.")

    except json.JSONDecodeError:
        print(f"\n❌ Błąd: Plik '{json_path}' nie jest prawidłowym plikiem JSON. Sprawdź, czy nie zawiera błędów składniowych.")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Wystąpił nieoczekiwany błąd: {e}")
        sys.exit(1)

if _name_ == "_main_":
    dodaj_regule_klasyfikacji()

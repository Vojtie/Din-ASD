/**
 *          ASD lab
 * Zadanie Zaliczeniowe 2 - DIN
 * Wojciech Kuzebski wk429552
 *         30.12.2021
 */

#include <iostream>
#include <algorithm>

using namespace std;

struct wezel {
    char litera;          
    char skr_lewa, skr_prawa;      // pierwsza/ostatnia litera w poddrzewie
    int ile_skr_lew, ile_skr_praw; // liczba takich samych liter w pre/sufiksie poddrzewa
    int wynik;                     // max liczba kolejnych identycznych liter w poddrzewie
    int prior;                     // priorytet wierzchołka
    int roz_drzewa;
    bool obrocone;                 // czy poddrzewo powinno zostać obrócone
    bool wszystkie_takie_same;     // czy wszystkie litery w poddrzewie są takie same
    wezel *l, *r;

    wezel(char val) : litera(val), prior(rand()) {
        obrocone = false;
        roz_drzewa = 1;
        l = r = nullptr;
        wszystkie_takie_same = true;
        skr_lewa = skr_prawa = litera;
        wynik = ile_skr_lew = ile_skr_praw = 1;
    }

};
// O(1)
int roz_drzewa(wezel *it) {
    return it ? it->roz_drzewa : 0;
}
// O(1)
int wynik(wezel *it) {
    return it ? it->wynik : 0;
}
// O(1)
void zepchnij(wezel *it) {
    if (it && it->obrocone) {
        it->obrocone = false;
        swap(it->l, it->r);
        swap(it->ile_skr_lew, it->ile_skr_praw);
        swap(it->skr_lewa, it->skr_prawa);
        if (it->l)
            it->l->obrocone ^= true;
        if (it->r)
            it->r->obrocone ^= true;
    }
}
// O(1)
void zaktualizuj(wezel *it) {
    if (it) {
        zepchnij(it->l), zepchnij(it->r);
        it->roz_drzewa = roz_drzewa(it->l) + roz_drzewa(it->r) + 1;
        if (it->l && it->l->wszystkie_takie_same && it->r && it->r->wszystkie_takie_same
            && it->litera == it->l->litera && it->litera == it->r->litera) {
            it->wszystkie_takie_same = true;
            it->skr_lewa = it->skr_prawa = it->litera;
            it->wynik = it->ile_skr_lew = it->ile_skr_praw = it->l->wynik + 1 + it->r->wynik;
        } else {
            it->wynik = 1;
            it->wszystkie_takie_same = false;
            it->ile_skr_lew = it->ile_skr_praw = -1;
            it->skr_lewa = it->skr_prawa = 'X';
            if (it->l) {
                it->skr_lewa = it->l->skr_lewa;
                if (it->litera == it->l->skr_prawa) {
                    it->wynik = max(it->wynik, it->l->ile_skr_praw + 1);
                    if (it->l->wszystkie_takie_same) {
                        it->ile_skr_lew = it->l->wynik + 1;
                        it->wszystkie_takie_same = true;
                    } else
                        it->ile_skr_lew = it->l->ile_skr_lew;
                } else
                    it->ile_skr_lew = it->l->ile_skr_lew;
            } else {
                it->skr_lewa = it->litera;
                if (it->r && it->r->skr_lewa == it->litera)
                    it->ile_skr_lew = it->r->ile_skr_lew + 1;
                else
                    it->ile_skr_lew = 1;
                it->wszystkie_takie_same = true;
            }
            if (it->r) {
                it->skr_prawa = it->r->skr_prawa;
                if (it->litera == it->r->skr_lewa) {
                    it->wynik = max(it->wynik, it->r->ile_skr_lew + 1);
                    if (it->r->wszystkie_takie_same)
                        it->ile_skr_praw = it->r->wynik + 1;
                    else
                        it->ile_skr_praw = it->r->ile_skr_praw, it->wszystkie_takie_same = false;
                } else
                    it->ile_skr_praw = it->r->ile_skr_praw, it->wszystkie_takie_same = false;
            } else {
                it->skr_prawa = it->litera;
                if (it->l && it->l->skr_prawa == it->litera)
                    it->ile_skr_praw = it->l->ile_skr_praw + 1;
                else
                    it->ile_skr_praw = 1;
            }
            if (it->l && it->r && it->litera == it->l->skr_prawa && it->litera == it->r->skr_lewa) {
                it->wynik = max(it->wynik, it->l->ile_skr_praw + it->r->ile_skr_lew + 1);
                it->skr_lewa = it->l->skr_lewa;
                it->skr_prawa = it->r->skr_prawa;
                if (it->l->wszystkie_takie_same)
                    it->ile_skr_lew = it->l->wynik + 1 + it->r->ile_skr_lew;
                else
                    it->ile_skr_lew = it->l->ile_skr_lew;
                if (it->r->wszystkie_takie_same)
                    it->ile_skr_praw = it->r->wynik + 1 + it->l->ile_skr_praw;
                else
                    it->ile_skr_praw = it->r->ile_skr_praw;
            }
            it->wynik = max({it->wynik, wynik(it->l), wynik(it->r), it->ile_skr_lew, it->ile_skr_praw});
        }
    }
}
// O(log(n))
void zlacz(wezel *&t, wezel *l, wezel *r) {
    zepchnij(l), zepchnij(r);
    if (!l || !r)
        t = l ? l : r;
    else if (l->prior > r->prior)
        zlacz(l->r, l->r, r), t = l;
    else
        zlacz(r->l, l, r->l), t = r;
    zaktualizuj(t);
}
// O(log(n))
void rozdziel(wezel *t, wezel *&l, wezel *&r, int klucz, int dod = 0) {
    if (!t)
        return void(l = r = nullptr);
    zepchnij(t);
    int t_klucz = dod + roz_drzewa(t->l);
    if (klucz <= t_klucz)
        rozdziel(t->l, l, t->l, klucz, dod), r = t;
    else
        rozdziel(t->r, t->r, r, klucz, dod + 1 + roz_drzewa(t->l)), l = t;
    zaktualizuj(t);
}

void heapify(wezel *t) {
    if (!t)
        return;
    wezel *max = t;
    if (t->l && t->l->prior > max->prior)
        max = t->l;
    if (t->r && t->r->prior > max->prior)
        max = t->r;
    if (max != t) {
        swap(t->prior, max->prior);
        heapify(max);
    }
}
// O(n)
wezel *zbuduj(char *a, int len) {
    if (len == 0)
        return nullptr;
    int mid = len / 2;
    auto t = new wezel(a[mid]);
    t->l = zbuduj(a, mid);
    t->r = zbuduj(a + mid + 1, len - mid - 1);
    heapify(t);
    zaktualizuj(t);
    return t;
}

// operacja O
// O(log(n))
void obroc(wezel *t, int l, int r) {
    wezel *t1, *t2, *t3;
    rozdziel(t, t1, t2, l);
    rozdziel(t2, t2, t3, r - l + 1);
    t2->obrocone ^= true;
    zlacz(t, t1, t2);
    zlacz(t, t, t3);
}

// operacja N
// O(log(n))
void wypisz_wynik(wezel *t, int l, int r) {
    wezel *t1, *t2, *t3;
    rozdziel(t, t1, t2, l);
    rozdziel(t2, t2, t3, r - l + 1);
    cout << wynik(t2) << endl;
    zlacz(t, t1, t2);
    zlacz(t, t, t3);
}

// operacja P
// O(log(n))
void przenies(wezel *t, int j, int k, int l) {
    wezel *t1, *t2, *t3, *t13, *t12;
    rozdziel(t, t1, t2, j);
    rozdziel(t2, t2, t3, k - j + 1);
    zlacz(t13, t1, t3);
    rozdziel(t13, t1, t3, l - 1);
    zlacz(t12, t1, t2);
    zlacz(t, t12, t3);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    char *ip = new char[n];
    for (int i = 0; i < n; i++)
        cin >> ip[i];

    wezel *t = zbuduj(ip, n);
    for (int i = 0; i < m; i++) {
        char operacja;
        int j, k, l;
        cin >> operacja >> j >> k;
        switch (operacja) {
            case 'O':
                obroc(t, j - 1, k - 1);
                break;
            case 'P':
                cin >> l;
                przenies(t, j - 1, k - 1, l);
                break;
            case 'N':
                wypisz_wynik(t, j - 1, k - 1);
                break;
            default:;
        }
    }
    delete[] ip;
}

/**
 * Problem: Clue Duel
 * Competition: IEEE Xtreme 20
 * Score: 65.37 / 66.88 (97.74%)
 */

import java.io.*;
import java.util.*;

public class Main {

    static final double TINY_EPS = 1e-4;

    static int[] size = new int[3];
    static int[] top = new int[3];

    static boolean[][] mine = new boolean[3][];
    static boolean[][] theirs = new boolean[3][];
    static boolean[][] shown = new boolean[3][];

    static double[][] score = new double[3][];
    static double[][] ownGuessBonus = new double[3][];

    static List<List<Integer>> fillers = new ArrayList<>();

    static int[] solution = new int[3];
    static int[] guess = new int[3];
    static boolean[] guessTested = new boolean[3];

    static int maxTurns;
    static int turns = 0;
    static int oppMultiMineTurns = 0;

    static class OppTurnRecord {
        int[] g = new int[3];
        boolean repliedNone;
        int shownCat;
    }

    static List<OppTurnRecord> oppHistory = new ArrayList<>();
    static Random rnd = new Random(42);

    static void send(String msg) {
        System.out.println(msg);
        System.out.flush();
    }

    static int cat(String card) {
        char c = card.charAt(0);
        return c == 'P' ? 0 : c == 'W' ? 1 : 2;
    }

    static int id(String card) {
        return Integer.parseInt(card.substring(1));
    }

    static String format(int p, int w, int l) {
        return "P" + p + " W" + w + " L" + l;
    }

    static void accuse(int p, int w, int l) {
        send("ACCUSE " + format(p, w, l));
    }

    // Çözümü kesinleşen kategorilerde diğer tüm kartları rakip kartı (theirs) olarak işaretle
    static void syncKnownSolutions() {
        for (int c = 0; c < 3; c++) {
            if (solution[c] == 0) {
                int last = 0, cnt = 0;
                for (int i = 1; i <= size[c]; i++) {
                    if (!mine[c][i] && !theirs[c][i]) {
                        last = i;
                        cnt++;
                    }
                }
                if (cnt == 1) {
                    solution[c] = last;
                }
            }
            if (solution[c] != 0) {
                for (int i = 1; i <= size[c]; i++) {
                    if (i != solution[c] && !mine[c][i]) {
                        theirs[c][i] = true;
                    }
                }
            }
        }
    }

    static List<Integer> candidates(int c) {
        syncKnownSolutions();
        List<Integer> list = new ArrayList<>();
        if (solution[c] != 0) {
            list.add(solution[c]);
            return list;
        }
        for (int i = 1; i <= size[c]; i++) {
            if (!mine[c][i] && !theirs[c][i]) {
                list.add(i);
            }
        }
        return list;
    }

    // Çözüm olma olasılığı (skoru) en yüksek adayı seç
    static int pickBest(int c, List<Integer> cand) {
        double bestScore = Double.NEGATIVE_INFINITY;
        List<Integer> bestList = new ArrayList<>();
        for (int i : cand) {
            double s = score[c][i] + ownGuessBonus[c][i];
            if (s > bestScore + 1e-9) {
                bestScore = s;
                bestList.clear();
                bestList.add(i);
            } else if (Math.abs(s - bestScore) <= 1e-9) {
                bestList.add(i);
            }
        }
        return bestList.get(rnd.nextInt(bestList.size()));
    }

    // Bilgi sızdırmamak için önce rakibe zaten gösterilmiş kartı, yoksa sabit tek bir kartı kullan
    static int filler(int c) {
        List<Integer> own = fillers.get(c);
        for (int id : own) {
            if (shown[c][id]) {
                return id;
            }
        }
        if (!own.isEmpty()) {
            return own.get(0);
        }
        return solution[c] != 0 ? solution[c] : 1;
    }

    static int revealed(int c) {
        int count = 0;
        for (int i = 1; i <= size[c]; i++) {
            if (shown[c][i]) {
                count++;
            }
        }
        return count;
    }

    static double accValue(int a, int b, int c) {
        return 3.0 + 7.0 / ((double) a * b * c);
    }

    static void myTurn() {
        syncKnownSolutions();

        List<List<Integer>> cand = new ArrayList<>();
        int[] n = new int[3];
        for (int c = 0; c < 3; c++) {
            cand.add(candidates(c));
            n[c] = cand.get(c).size();
            if (n[c] == 0) {
                accuse(1, 1, 1);
                return;
            }
        }

        // Çözüm kesinleştiyse hemen suçla
        if (n[0] == 1 && n[1] == 1 && n[2] == 1) {
            accuse(cand.get(0).get(0), cand.get(1).get(0), cand.get(2).get(0));
            return;
        }

        // Rakibin geçmiş hamlelerinden aday skorlarını ve rakibin çözüm durumunu hesapla
        for (int c = 0; c < 3; c++) {
            Arrays.fill(score[c], 0.0);
        }

        int[] unrev = new int[3];
        double[] oppSolved = new double[3];
        for (int c = 0; c < 3; c++) {
            unrev[c] = Math.max(1, top[c] - revealed(c));
            if (unrev[c] == 1) {
                oppSolved[c] = 1.0;
            }
        }

        boolean oppIsFillerBot = (oppMultiMineTurns == 0);
        boolean[][] seenShown = new boolean[3][45];

        for (OppTurnRecord rec : oppHistory) {
            if (rec.repliedNone) {
                List<Integer> unknownCats = new ArrayList<>();
                boolean alreadyHasKnownSol = false;
                for (int c = 0; c < 3; c++) {
                    int id = rec.g[c];
                    if (solution[c] == id) {
                        oppSolved[c] = 1.0;
                        alreadyHasKnownSol = true;
                    } else if (!theirs[c][id]) {
                        unknownCats.add(c);
                    }
                }
                if (!unknownCats.isEmpty()) {
                    double totalLikelySolved = (oppIsFillerBot && alreadyHasKnownSol) ? 0.25 : 1.0;
                    double pSolve = totalLikelySolved / unknownCats.size();
                    for (int c : unknownCats) {
                        oppSolved[c] = 1.0 - (1.0 - oppSolved[c]) * (1.0 - pSolve);
                        score[c][rec.g[c]] += 2.5 * pSolve;
                    }
                }
            } else {
                int mCount = 0;
                for (int c = 0; c < 3; c++) {
                    int id = rec.g[c];
                    if (mine[c][id]) {
                        mCount++;
                        // Rakip elimizdeki gösterilmemiş bir kartı denediyse o kategoriyi henüz çözmemiştir
                        if (!seenShown[c][id] && unrev[c] > 1) {
                            oppSolved[c] = 0.0;
                        }
                    }
                }
                if (rec.shownCat >= 0) {
                    seenShown[rec.shownCat][rec.g[rec.shownCat]] = true;
                }
                if (oppIsFillerBot && mCount == 1) {
                    for (int c = 0; c < 3; c++) {
                        int id = rec.g[c];
                        if (!mine[c][id] && oppSolved[c] < 0.5 && unrev[c] > 1) {
                            score[c][id] -= 1.0; // Rakibin kendi elinden kullandığı olası filler
                        }
                    }
                } else if (!oppIsFillerBot && mCount == 2) {
                    for (int c = 0; c < 3; c++) {
                        int id = rec.g[c];
                        if (!mine[c][id]) {
                            score[c][id] += 0.5;
                        }
                    }
                }
            }
        }

        for (int c = 0; c < 3; c++) {
            if (unrev[c] == 1) {
                oppSolved[c] = 1.0;
            }
        }

        int myTurnsLeft = (maxTurns - turns) / 2 + 1;
        if (myTurnsLeft <= 1) {
            accuse(
                    pickBest(0, cand.get(0)),
                    pickBest(1, cand.get(1)),
                    pickBest(2, cand.get(2))
            );
            return;
        }

        int maxSteps = Math.min(myTurnsLeft - 1, (n[0] - 1) + (n[1] - 1) + (n[2] - 1));

        // Rakibin mevcut gerçek durumundan ileriye dönük bitirme olasılığı dağılımı
        int u0 = unrev[0], u1 = unrev[1], u2 = unrev[2];
        double[][][] oppProb = new double[u0 + 1][u1 + 1][u2 + 1];
        double[] p0 = {1.0 - oppSolved[0], oppSolved[0]};
        double[] p1 = {1.0 - oppSolved[1], oppSolved[1]};
        double[] p2 = {1.0 - oppSolved[2], oppSolved[2]};

        for (int i0 = 0; i0 < 2; i0++) {
            int a = (i0 == 1) ? 1 : u0;
            for (int i1 = 0; i1 < 2; i1++) {
                int b = (i1 == 1) ? 1 : u1;
                for (int i2 = 0; i2 < 2; i2++) {
                    int c = (i2 == 1) ? 1 : u2;
                    oppProb[a][b][c] += p0[i0] * p1[i1] * p2[i2];
                }
            }
        }

        double[] done = new double[maxSteps + 1];
        done[0] = oppProb[1][1][1];

        for (int s = 1; s <= maxSteps; s++) {
            double[][][] nextProb = new double[u0 + 1][u1 + 1][u2 + 1];
            for (int a = 1; a <= u0; a++) {
                for (int b = 1; b <= u1; b++) {
                    for (int c = 1; c <= u2; c++) {
                        double p = oppProb[a][b][c];
                        if (p == 0) continue;
                        if (a == 1 && b == 1 && c == 1) {
                            nextProb[1][1][1] += p;
                            continue;
                        }
                        int[] arr = {a, b, c};
                        int k = 0;
                        for (int i = 1; i < 3; i++) {
                            if (arr[i] > arr[k]) k = i;
                        }
                        double q = 1.0 / arr[k];
                        int[] st = arr.clone();
                        st[k] = 1;
                        nextProb[st[0]][st[1]][st[2]] += p * q;
                        st[k] = arr[k] - 1;
                        nextProb[st[0]][st[1]][st[2]] += p * (1.0 - q);
                    }
                }
            }
            oppProb = nextProb;
            done[s] = oppProb[1][1][1];
        }

        // Düzeltilmiş tur bazlı tehlike oranı (hazard):
        // s adımındaki rakip turunda suçlama gelmesi, rakibin s. tahminden ÖNCE (1,1,1)'e ulaşmış olmasına bağlıdır
        double[] h = new double[maxSteps];
        for (int s = 0; s < maxSteps; s++) {
            double before = (s == 0) ? 0.0 : done[s - 1];
            double upto = done[s];
            double cond = 0.0;
            if (1.0 - before > 1e-9) {
                cond = (upto - before) / (1.0 - before);
            } else {
                cond = 1.0;
            }
            cond = Math.max(0.0, Math.min(1.0, cond));
            h[s] = 1.0 - (1.0 - cond) * (1.0 - TINY_EPS);
        }

        // Geriye doğru Dinamik Programlama (1, 2 ve 3 bilinmeyenli tüm tahmin maskeleri dahil)
        double[][][] nextVal = new double[n[0] + 1][n[1] + 1][n[2] + 1];
        double[][][] curVal = new double[n[0] + 1][n[1] + 1][n[2] + 1];

        for (int a = 1; a <= n[0]; a++) {
            for (int b = 1; b <= n[1]; b++) {
                for (int c = 1; c <= n[2]; c++) {
                    nextVal[a][b][c] = accValue(a, b, c);
                }
            }
        }

        int bestMask = 0;

        for (int s = maxSteps - 1; s >= 0; s--) {
            double survive = 1.0 - h[s];
            for (int a = 1; a <= n[0]; a++) {
                for (int b = 1; b <= n[1]; b++) {
                    for (int c = 1; c <= n[2]; c++) {
                        double best = accValue(a, b, c);
                        int maskChoice = 0;

                        // 1 bilinmeyenli testler (Mask 1, 2, 4)
                        if (a > 1) {
                            double v = survive * (nextVal[1][b][c] / a + nextVal[a - 1][b][c] * (a - 1.0) / a);
                            if (v > best) { best = v; maskChoice = 1; }
                        }
                        if (b > 1) {
                            double v = survive * (nextVal[a][1][c] / b + nextVal[a][b - 1][c] * (b - 1.0) / b);
                            if (v > best) { best = v; maskChoice = 2; }
                        }
                        if (c > 1) {
                            double v = survive * (nextVal[a][b][1] / c + nextVal[a][b][c - 1] * (c - 1.0) / c);
                            if (v > best) { best = v; maskChoice = 4; }
                        }

                        // 2 bilinmeyenli testler (Mask 3, 5, 6)
                        if (a > 1 && b > 1) {
                            double v00 = nextVal[1][1][c];
                            double v10 = nextVal[a - 1][b][c];
                            double v01 = nextVal[a][b - 1][c];
                            double v11 = 0.5 * Math.min(v10, v01) + 0.25 * (v10 + v01);
                            double v = survive * (v00 + (a - 1.0) * v10 + (b - 1.0) * v01
                                    + (a - 1.0) * (b - 1.0) * v11) / ((double) a * b);
                            if (v > best) { best = v; maskChoice = 3; }
                        }
                        if (a > 1 && c > 1) {
                            double v00 = nextVal[1][b][1];
                            double v10 = nextVal[a - 1][b][c];
                            double v01 = nextVal[a][b][c - 1];
                            double v11 = 0.5 * Math.min(v10, v01) + 0.25 * (v10 + v01);
                            double v = survive * (v00 + (a - 1.0) * v10 + (c - 1.0) * v01
                                    + (a - 1.0) * (c - 1.0) * v11) / ((double) a * c);
                            if (v > best) { best = v; maskChoice = 5; }
                        }
                        if (b > 1 && c > 1) {
                            double v00 = nextVal[a][1][1];
                            double v10 = nextVal[a][b - 1][c];
                            double v01 = nextVal[a][b][c - 1];
                            double v11 = 0.5 * Math.min(v10, v01) + 0.25 * (v10 + v01);
                            double v = survive * (v00 + (b - 1.0) * v10 + (c - 1.0) * v01
                                    + (b - 1.0) * (c - 1.0) * v11) / ((double) b * c);
                            if (v > best) { best = v; maskChoice = 6; }
                        }

                        // 3 bilinmeyenli test (Mask 7)
                        if (a > 1 && b > 1 && c > 1) {
                            double v000 = nextVal[1][1][1];
                            double va = nextVal[a - 1][b][c];
                            double vb = nextVal[a][b - 1][c];
                            double vc = nextVal[a][b][c - 1];
                            double vab = 0.5 * Math.min(va, vb) + 0.25 * (va + vb);
                            double vac = 0.5 * Math.min(va, vc) + 0.25 * (va + vc);
                            double vbc = 0.5 * Math.min(vb, vc) + 0.25 * (vb + vc);
                            double vabc = 0.5 * Math.min(va, Math.min(vb, vc)) + (va + vb + vc) / 6.0;
                            double num = v000
                                    + (a - 1.0) * va + (b - 1.0) * vb + (c - 1.0) * vc
                                    + (a - 1.0) * (b - 1.0) * vab
                                    + (a - 1.0) * (c - 1.0) * vac
                                    + (b - 1.0) * (c - 1.0) * vbc
                                    + (a - 1.0) * (b - 1.0) * (c - 1.0) * vabc;
                            double v = survive * num / ((double) a * b * c);
                            if (v > best) { best = v; maskChoice = 7; }
                        }

                        curVal[a][b][c] = best;
                        if (s == 0 && a == n[0] && b == n[1] && c == n[2]) {
                            bestMask = maskChoice;
                        }
                    }
                }
            }
            double[][][] tmp = nextVal;
            nextVal = curVal;
            curVal = tmp;
        }

        if (bestMask == 0) {
            accuse(
                    pickBest(0, cand.get(0)),
                    pickBest(1, cand.get(1)),
                    pickBest(2, cand.get(2))
            );
            return;
        }

        for (int c = 0; c < 3; c++) {
            if ((bestMask & (1 << c)) != 0) {
                guess[c] = pickBest(c, cand.get(c));
                guessTested[c] = true;
            } else {
                guess[c] = filler(c);
                guessTested[c] = false;
            }
        }

        send("GUESS " + format(guess[0], guess[1], guess[2]));
    }

    static void opponentTurn(List<String> t) {
        String[] cards = {t.get(3), t.get(4), t.get(5)};

        OppTurnRecord rec = new OppTurnRecord();
        int mCount = 0;
        for (int k = 0; k < 3; k++) {
            int c = cat(cards[k]);
            int i = id(cards[k]);
            rec.g[c] = i;
            if (mine[c][i]) {
                mCount++;
            }
        }
        if (mCount >= 2) {
            oppMultiMineTurns++;
        }

        String chosen = null;
        int bestScore = -1;

        for (String card : cards) {
            int c = cat(card);
            int i = id(card);

            if (!mine[c][i]) {
                continue;
            }

            // Daha önce gösterdiğimiz kartı tekrar göstererek rakibe 0 yeni bilgi ver
            if (shown[c][i]) {
                chosen = card;
                break;
            }

            int remaining = fillers.get(c).size() - revealed(c);
            // Eşitlik durumunda kendi sabit filler kartımızı göstermeyi tercih et (ekstra kart ifşa etmez)
            int isPrimaryFiller = (!fillers.get(c).isEmpty() && fillers.get(c).get(0) == i) ? 1 : 0;
            int curScore = remaining * 2 + isPrimaryFiller;

            if (curScore > bestScore) {
                bestScore = curScore;
                chosen = card;
            }
        }

        if (chosen == null) {
            rec.repliedNone = true;
            rec.shownCat = -1;
            oppHistory.add(rec);
            send("NONE");
            return;
        }

        int sc = cat(chosen);
        int sid = id(chosen);
        rec.repliedNone = false;
        rec.shownCat = sc;
        oppHistory.add(rec);

        shown[sc][sid] = true;
        send("SHOW " + chosen);
    }

    static void result(List<String> t) {
        if (t.get(1).equals("SHOWN")) {
            String card = t.get(2);
            int sc = cat(card);
            int sid = id(card);
            theirs[sc][sid] = true;

            // Çoklu bilinmeyen denediğimizde gösterilmeyen diğer adaylara Bayesyen öncelik bonusu ver
            int testedCount = 0;
            for (int c = 0; c < 3; c++) {
                if (guessTested[c]) testedCount++;
            }
            if (testedCount >= 2) {
                for (int c = 0; c < 3; c++) {
                    if (guessTested[c] && c != sc) {
                        ownGuessBonus[c][guess[c]] += 0.35;
                    }
                }
            }
        } else if (t.get(1).equals("NONE")) {
            for (int c = 0; c < 3; c++) {
                if (guess[c] != 0 && !mine[c][guess[c]]) {
                    solution[c] = guess[c];
                }
            }
        }
        syncKnownSolutions();
    }

    public static void main(String[] args) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        String firstLine = in.readLine();
        if (firstLine == null) return;

        StringTokenizer st = new StringTokenizer(firstLine);
        st.nextToken(); // GAME

        for (int i = 0; i < 3; i++) {
            size[i] = Integer.parseInt(st.nextToken());
            top[i] = (size[i] + 1) / 2;

            mine[i] = new boolean[size[i] + 1];
            theirs[i] = new boolean[size[i] + 1];
            shown[i] = new boolean[size[i] + 1];
            score[i] = new double[size[i] + 1];
            ownGuessBonus[i] = new double[size[i] + 1];

            fillers.add(new ArrayList<>());
        }

        maxTurns = Integer.parseInt(st.nextToken());

        st = new StringTokenizer(in.readLine());
        st.nextToken(); // HAND

        while (st.hasMoreTokens()) {
            String card = st.nextToken();
            int c = cat(card);
            int i = id(card);
            mine[c][i] = true;
            fillers.get(c).add(i);
        }

        for (List<Integer> list : fillers) {
            Collections.shuffle(list, rnd);
        }

        String line;
        while ((line = in.readLine()) != null) {
            st = new StringTokenizer(line);
            if (!st.hasMoreTokens()) {
                continue;
            }

            List<String> t = new ArrayList<>();
            while (st.hasMoreTokens()) {
                t.add(st.nextToken());
            }

            String cmd = t.get(0);
            if (cmd.equals("GAME_END")) {
                break;
            } else if (cmd.equals("TURN")) {
                turns++;
                if (t.get(1).equals("YOU")) {
                    myTurn();
                } else {
                    opponentTurn(t);
                }
            } else if (cmd.equals("RESULT")) {
                result(t);
            }
        }
    }
}
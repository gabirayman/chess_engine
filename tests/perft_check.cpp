#include "chess/board.hpp"
#include "chess/fen.hpp"
#include "chess/perft.hpp"

#include <cctype>
#include <fstream> 
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

using namespace chess;

struct ExpectedRow {
    int depth = 0;
    PerftStats stats{};
};

struct Case {
    std::string name;
    std::string fen;
    std::vector<ExpectedRow> rows; // D1..DN
};

static void usage() {
    std::cout <<
"Usage:\n"
"  perft_check --file tests/data/perft_cases.txt --depth 4\n";
}

// Trim helpers
static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

static bool hasFlag(const std::vector<std::string>& args, const std::string& key) {
    for (const auto& a : args) if (a == key) return true;
    return false;
}


static bool parse_file(const std::string& path, std::vector<Case>& out) {
    std::ifstream in(path);
    if (!in) return false;

    Case cur;
    std::string line;

    auto flush_case = [&]() {
        if (!cur.fen.empty()) out.push_back(cur);
        cur = Case{};
    };

    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty() || t[0] == '#') continue;

        if (t.rfind("name:", 0) == 0) {
            flush_case();
            cur.name = trim(t.substr(5));
        } else if (t.rfind("fen:", 0) == 0) {
            cur.fen = trim(t.substr(4));
        } else if (t.rfind("D", 0) == 0) {
            // Expect Dn: nodes captures ep castles promotions checks mates

            std::istringstream ss(t);
            char D; int depth; char colon;
            ExpectedRow row;
            if (!(ss >> D >> depth >> colon)) continue; // skip if malformed

            row.depth = depth;
            if (!(ss >> row.stats.nodes
                    >> row.stats.captures
                    >> row.stats.enPassant
                    >> row.stats.castles
                    >> row.stats.promotions
                    >> row.stats.checks
                    >> row.stats.checkmates)) {
                std::cerr << "Bad D" << depth << " line: " << t << "\n";
                continue;
            }
            cur.rows.push_back(row);
        }
    }
    flush_case();
    return true;
}

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::string filePath, depthStr;

    bool timing = hasFlag(args, "--time");

    for (size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == "--file")  filePath = args[i + 1];
        if (args[i] == "--depth") depthStr = args[i + 1];
    }
    if (filePath.empty() || depthStr.empty()) {
        usage();
        return 1;
    }
    int maxDepth = std::stoi(depthStr);

    std::vector<Case> cases;
    if (!parse_file(filePath, cases)) {
        std::cerr << "Cannot open/parse: " << filePath << "\n";
        return 1;
    }

    std::size_t passed = 0, failed = 0;

    for (const auto& c : cases) {
        Board b;
        if (!setFromFEN(b, c.fen)) {
            std::cerr << "[BAD FEN] " << (c.name.empty() ? "<noname>" : c.name) << "\n";
            failed++;
            continue;
        }

        // Index expected rows by depth for quick lookup
        std::vector<PerftStats> exp(maxDepth + 1);
        std::vector<bool>       has(maxDepth + 1, false);
        for (const auto& r : c.rows) {
            if (1 <= r.depth && r.depth <= maxDepth) {
                exp[r.depth] = r.stats;
                has[r.depth] = true;
            }
        }

        bool ok = true;
        for (int d = 1; d <= maxDepth; ++d) {
            if (!has[d]) continue; // no expectations for this depth, skip

            auto t0 = std::chrono::steady_clock::now();
            PerftStats got = perft_stats(b, d);
            auto t1 = std::chrono::steady_clock::now();

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
            if (timing) {
                double sec = ms / 1000.0;
                double nps = (sec > 0.0) ? (static_cast<double>(got.nodes) / sec) : 0.0;
                std::cout << "  D" << d << ": " << ms << " ms"
                        << "  nodes=" << got.nodes
                        << "  nps=" << static_cast<long long>(nps) << "\n";
            }

            const auto& e  = exp[d];

            bool thisDepthOk =
                got.nodes      == e.nodes      &&
                got.captures   == e.captures   &&
                got.enPassant  == e.enPassant  &&
                got.castles    == e.castles    &&
                got.promotions == e.promotions &&
                got.checks     == e.checks     &&
                got.checkmates == e.checkmates;

            if (!thisDepthOk) {
                ok = false;
                std::cerr << "[FAIL] " << (c.name.empty() ? "<noname>" : c.name)
                          << " D" << d << "\n"
                          << "  expected: N=" << e.nodes
                          << " C=" << e.captures
                          << " EP=" << e.enPassant
                          << " Ca=" << e.castles
                          << " P=" << e.promotions
                          << " Ch=" << e.checks
                          << " CM=" << e.checkmates << "\n"
                          << "  got:      N=" << got.nodes
                          << " C=" << got.captures
                          << " EP=" << got.enPassant
                          << " Ca=" << got.castles
                          << " P=" << got.promotions
                          << " Ch=" << got.checks
                          << " CM=" << got.checkmates << "\n";
            }
        }

        if (ok) {
            passed++;
            std::cout << "[PASS] " << (c.name.empty() ? "<noname>" : c.name) << "\n";
        } else {
            failed++;
        }
    }

    std::cout << "Summary: pass=" << passed << " fail=" << failed << "\n";
    return failed ? 1 : 0;
}

#include "gui/T4CCharacterQuestionnaire.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdio>
#include <random>

namespace {

struct QuestionBankEntry {
    const char *question;
    const char *answers[5];
};

/** Textes FR (indices LocalString 106+i*6, 107..111+i*6). Source : client 1.68 / guides communautaires. */
constexpr QuestionBankEntry kFrenchBank[] = {
    {"Un reve premonitoire a trouble votre sommeil. Vous avez reve qu'un...",
     {"scarabee doré venait se poser sur votre front.",
      "hyene riait aux eclats de votre depouille.",
      "hibou blanc vous fixait d'un regard glacial.",
      "corbeau croassait au-dessus de votre lit.",
      "geai venait picorer vos cheveux."}},
    {"Un menestrel vous offre votre ballade preferee. Que lui demandez-vous ?",
     {"les aventures du chevalier Boeris.",
      "les exploits de Morgan le Preste.",
      "la vie du saint Pierre.",
      "les sortileges de l'archimage Salvieri.",
      "l'histoire du Corbeau et du Cygne."}},
    {"Qu'est-ce qui vous est le plus cher ?",
     {"votre force de caractere.",
      "votre liberte.",
      "vos amis et votre famille.",
      "vos yeux et vos mains.",
      "un crapaud nomme Jeremiah."}},
    {"Le Roi est vieux et ruse. Pour son anniversaire, il vous demande un breuvage. Que lui preparez-vous ?",
     {"une biere forte.",
      "un poison discret.",
      "une potion de rajeunissement.",
      "de l'ambroisie celeste.",
      "un simple verre d'eau."}},
    {"La nuit precedant votre rite de passage, un messager venu du ciel vous est apparu. Qui etait-il ?",
     {"une valkyrie aux cheveux d'or.",
      "une jeune fille aux yeux de jade.",
      "une figure vetue de samite blanc.",
      "une gargouille aux ailes de cuir.",
      "un geant aux pas de tonnerre."}},
    {"Tous se croient dignes, mais personne ne s'entend sur le sens de la dignite. Qui est le plus digne ?",
     {"une amazone guerriere.",
      "un homme qui fuit le combat.",
      "un homme humble et discret.",
      "un roi astucieux.",
      "un troubadour errant."}},
    {"Vous entrez dans une taverne bruyante. Que faites-vous en premier ?",
     {"commander le plus fort en alcool.",
      "observer qui triche aux des.",
      "ecouter les rumeurs du comptoir.",
      "regarder qui entre et qui sort.",
      "chercher un coin tranquille pour lire."}},
    {"Un etranger vous propose un pacte. Que retenez-vous ?",
     {"la gloire au combat.",
      "le savoir interdit.",
      "la protection des faibles.",
      "l'or et les richesses.",
      "la liberte sans maitre."}},
};

static_assert(sizeof(kFrenchBank) / sizeof(kFrenchBank[0]) == T4CCharacterQuestionnaire::kQuestionBankSize,
              "question bank size");

unsigned int ShuffleSeed() {
#if defined(_WIN32)
    return static_cast<unsigned int>(timeGetTime());
#else
    return static_cast<unsigned int>(SDL_GetTicks());
#endif
}

int RandInclusive(int lo, int hi, std::mt19937 &rng) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng);
}

}  // namespace

void T4CCharacterQuestionnaire::shuffle() {
    std::mt19937 rng(ShuffleSeed());

    for (auto &row : rn_) {
        row.fill(5);
    }
    qn_.fill(6);

    for (int i = 0; i < kQuestionBankSize; ++i) {
        qn_[i] = RandInclusive(0, 7, rng);
        bool okay = true;
        for (int j = 0; j < i; ++j) {
            if (qn_[i] == qn_[j]) {
                okay = false;
                break;
            }
        }
        if (!okay) {
            --i;
        }
    }

    for (int k = 0; k < kQuestionBankSize; ++k) {
        for (int i = 0; i < kAnswerCount; ++i) {
            rn_[qn_[k]][i] = RandInclusive(0, 4, rng);
            bool okay = true;
            for (int j = 0; j < i; ++j) {
                if (rn_[qn_[k]][i] == rn_[qn_[k]][j]) {
                    okay = false;
                    break;
                }
            }
            if (!okay) {
                --i;
            }
        }
    }
}

void T4CCharacterQuestionnaire::reset() {
    questionNumber_ = 0;
    selectedChoice_ = 1;
    answers_.fill(0);
    shuffle();
}

std::string T4CCharacterQuestionnaire::questionText() const {
    if (questionNumber_ < 0 || questionNumber_ >= kQuestionsAsked) {
        return {};
    }
    const int bankId = qn_[questionNumber_];
    if (bankId < 0 || bankId >= kQuestionBankSize) {
        return {};
    }
    return kFrenchBank[bankId].question;
}

std::string T4CCharacterQuestionnaire::answerText(const int displaySlot) const {
    if (questionNumber_ < 0 || questionNumber_ >= kQuestionsAsked) {
        return {};
    }
    if (displaySlot < 0 || displaySlot >= kAnswerCount) {
        return {};
    }
    const int bankId = qn_[questionNumber_];
    if (bankId < 0 || bankId >= kQuestionBankSize) {
        return {};
    }
    const int answerId = rn_[questionNumber_][displaySlot];
    if (answerId < 0 || answerId >= kAnswerCount) {
        return {};
    }
    return kFrenchBank[bankId].answers[answerId];
}

void T4CCharacterQuestionnaire::setSelectedChoice(const int choice1to5) {
    selectedChoice_ = std::clamp(choice1to5, 1, kAnswerCount);
}

bool T4CCharacterQuestionnaire::confirmChoice() {
    if (questionNumber_ >= kQuestionsAsked) {
        return true;
    }
    const int statIndex = rn_[questionNumber_][selectedChoice_ - 1];
    if (statIndex >= 0 && statIndex < kAnswerCount) {
        ++answers_[static_cast<std::size_t>(statIndex)];
    }
    ++questionNumber_;
    selectedChoice_ = 1;
    return questionNumber_ >= kQuestionsAsked;
}

void T4CCharacterQuestionnaire::fillPacketStats(unsigned char out[5]) const {
    if (!out) {
        return;
    }
    out[0] = answers_[3];
    out[1] = answers_[2];
    out[2] = answers_[0];
    out[3] = answers_[1];
    out[4] = answers_[4];
}

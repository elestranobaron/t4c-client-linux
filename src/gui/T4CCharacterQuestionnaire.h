#pragma once

#include <array>
#include <cstdint>
#include <string>

/** Questionnaire creation perso (aligne TFCSocket.cpp : Shuffle, QN, RN, QuestionAnswer). */
class T4CCharacterQuestionnaire {
   public:
    static constexpr int kQuestionBankSize = 8;
    static constexpr int kAnswerCount = 5;
    static constexpr int kQuestionsAsked = 4;

    void reset();

    int questionNumber() const { return questionNumber_; }
    bool isComplete() const { return questionNumber_ >= kQuestionsAsked; }

    std::string questionText() const;
    std::string answerText(int displaySlot) const;

    int selectedChoice() const { return selectedChoice_; }
    void setSelectedChoice(int choice1to5);

    /** Valide la reponse courante ; retourne true si les 4 questions sont finies. */
    bool confirmChoice();

    /** Octets opcode 25 : ordre Windows QuestionAnswer[3,2,0,1,4]. */
    void fillPacketStats(unsigned char out[5]) const;

   private:
    void shuffle();

    int questionNumber_{0};
    int selectedChoice_{1};
    std::array<unsigned char, 5> answers_{};
    std::array<int, kQuestionBankSize> qn_{};
    std::array<std::array<int, kAnswerCount>, kQuestionBankSize> rn_{};
};

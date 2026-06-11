#include "gui/IntroductionScreen.h"

#include "gui/LauncherChrome.h"
#include "gui/T4CUiFont.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <string>

namespace {

constexpr float kPaddingX = 48.0f;
constexpr float kPaddingTop = 72.0f;
constexpr float kMaxTextWidth = 704.0f;
constexpr float kLineSpacing = 6.0f;
constexpr float kParagraphGap = 18.0f;
constexpr float kAutoScrollPxPerSec = 28.0f;

const char *kStoryParagraphs[] = {
    "Conte de la Quatrieme Venue",
    "",
    "Il est des recits d'un age ancien, epoque ou les Elfes peuplaient le monde en grand "
    "nombre, ou leurs exploits surpassaient l'eclat du soleil, ou les civilisations humaines "
    "et naines n'en etaient qu'a leurs balbutiements. Ce fut un temps de legendes et de heros, "
    "d'accomplissements incroyables et de grands faits, un temps ou le monde connaissait une "
    "veritable grandeur. Ce temps est depuis longtemps passe, ecrase sous le talon du destin "
    "et de la decadence.",
    "",
    "Les Elfes ont disparu, victimes de leurs propres illusions. Ils n'avaient pas ecoute "
    "l'Haruspice, celui qui etait venu les avertir... Il arriva par une nuit ou la lune et la "
    "constellation du Centaure etaient alignees, il y a plusieurs millenaires, son apparence "
    "infecte et cauchemardesque, et il mit en garde tous ceux qui voulaient l'entendre contre "
    "le mal imminent. Il prononca des avertissements et des propheties, mais les Elfes etaient "
    "devenus vaniteux et arrogants, et ne l'ecouterent point. L'Haruspice partit, promettant de "
    "revenir lorsque le temps serait de nouveau propice.",
    "",
    "Plusieurs generations plus tard — un bref instant a l'echelle elfique — l'Haruspice revint, "
    "une fois de plus sous l'alignement des lunes et des constellations. Les Elfes avaient "
    "presque oublie ses avertissements passes. Lorsque la Malediction s'abattit sur leur race, "
    "ils se trouverent sans defense. Malgre leur savoir arcanique et leurs talents magiques, ils "
    "ne purent resister aux pouvoirs divins qui les ecraserent. Quand l'Haruspice quitta leurs "
    "terres, il ne restait pas un seul batiment debout. On dit que les vents portaient encore la "
    "puanteur de la mort jusqu'aux villages nains, au nord.",
    "",
    "Ceux-ci prirent cela pour un signe que le mal venait, et se preparerent a se defendre. "
    "Quand l'Haruspice vint a leur tour les avertir que leur heure viendrait, que grand peril "
    "les attendait, ils furent effrayes par l'apparence du visiteur, et le repousserent. Il les "
    "quitta en leur disant que les exploits seuls ne faisaient pas la mesure d'un peuple digne.",
    "",
    "Un millenaire plus tard, les cieux repeterent leur fatale alignement. Les Nains avaient "
    "prospere en une societe d'artisans. Ils avaient bati de grandes cites et veneraient "
    "pieusement leur dieu. Des propheties ancestrales les avaient mis en garde contre un sort "
    "semblable a celui des Elfes, et ils avaient pris soin de se preparer a la Troisieme Venue. "
    "Quand l'Haruspice vint a eux, ils ne purent supporter ni la vue ni l'odeur de celui-ci, et, "
    "le prenant pour un demon venu des enfers, le frapperent sans tarder. Cet acte, disent les "
    "historiens, fut precisement celui qui causa la chute des Nains. Ils disent aussi que des "
    "humains furent temoins de l'evenement, et que l'Haruspice les avertit egalement, leur "
    "annoncant qu'eux aussi seraient juges, a moins que leur valeur ne surpassat celle des Elfes "
    "et des Nains.",
    "",
    "Peut-etre n'est-il rien de vrai dans ce conte, peut-etre ne sommes-nous qu'un peuple "
    "inquiet vivant sous un alignement insolite des etoiles, qui ne s'etait pas produit depuis "
    "mille ans. Mais peut-etre, seulement peut-etre, y a-t-il quelque verite dans cette "
    "histoire. Peut-etre qu'il existe vraiment un Haruspice qui parcourt la terre en ce moment "
    "meme, marchant vers nous pour nous juger et nous tourmenter si nous lui faisons defaut.",
    "",
    "La Quatrieme Venue est celle des Humains. Es-tu digne de l'accueillir ?",
};

bool KeyDown(const SDL_Event &event, SDL_Scancode sc) {
    return event.type == SDL_EVENT_KEY_DOWN && event.key.down && !event.key.repeat &&
           event.key.scancode == sc;
}

void AppendWord(std::string &line, const std::string &word) {
    if (line.empty()) {
        line = word;
        return;
    }
    line.push_back(' ');
    line.append(word);
}

std::vector<std::string> WrapParagraphAscii(const std::string &paragraph, std::size_t maxChars) {
    std::vector<std::string> out;
    if (paragraph.empty()) {
        out.emplace_back();
        return out;
    }
    std::string line;
    std::string word;
    for (unsigned char c : paragraph) {
        if (std::isspace(c)) {
            if (!word.empty()) {
                if (line.empty()) {
                    AppendWord(line, word);
                } else if (line.size() + 1 + word.size() <= maxChars) {
                    AppendWord(line, word);
                } else {
                    out.push_back(line);
                    line = word;
                }
                word.clear();
            }
        } else {
            word.push_back(static_cast<char>(c));
        }
    }
    if (!word.empty()) {
        if (line.empty() || line.size() + 1 + word.size() <= maxChars) {
            AppendWord(line, word);
        } else {
            out.push_back(line);
            line = word;
        }
    }
    if (!line.empty()) {
        out.push_back(line);
    }
    return out;
}

std::vector<std::string> WrapParagraphFont(const std::string &paragraph, const T4CUiFont &font,
                                           float maxWidth) {
    std::vector<std::string> out;
    if (paragraph.empty()) {
        out.emplace_back();
        return out;
    }
    std::string line;
    std::string word;
    auto flushWord = [&]() {
        if (word.empty()) {
            return;
        }
        std::string candidate = line;
        AppendWord(candidate, word);
        int w = 0;
        font.measureText(candidate.c_str(), &w, nullptr);
        if (line.empty() || static_cast<float>(w) <= maxWidth) {
            line = std::move(candidate);
        } else {
            out.push_back(line);
            line = word;
        }
        word.clear();
    };

    for (unsigned char c : paragraph) {
        if (std::isspace(c)) {
            flushWord();
        } else {
            word.push_back(static_cast<char>(c));
        }
    }
    flushWord();
    if (!line.empty()) {
        out.push_back(line);
    }
    return out;
}

}  // namespace

void IntroductionScreen::open() {
    open_ = true;
    scrollY_ = 0.0f;
    linesDirty_ = true;
}

void IntroductionScreen::close() {
    open_ = false;
}

bool IntroductionScreen::HandleEvent(const SDL_Event &event) {
    if (!open_) {
        return true;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            if (KeyDown(event, SDL_SCANCODE_ESCAPE) || KeyDown(event, SDL_SCANCODE_RETURN) ||
                KeyDown(event, SDL_SCANCODE_SPACE)) {
                close();
                return false;
            }
            if (KeyDown(event, SDL_SCANCODE_UP)) {
                scrollY_ = std::max(0.0f, scrollY_ - 48.0f);
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_DOWN)) {
                scrollY_ += 48.0f;
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_PAGEUP)) {
                scrollY_ = std::max(0.0f, scrollY_ - 220.0f);
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_PAGEDOWN)) {
                scrollY_ += 220.0f;
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_HOME)) {
                scrollY_ = 0.0f;
                return true;
            }
            return true;
        case SDL_EVENT_MOUSE_WHEEL:
            scrollY_ = std::max(0.0f, scrollY_ - event.wheel.y * 36.0f);
            return true;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                close();
                return false;
            }
            return true;
        default:
            return true;
    }
}

void IntroductionScreen::Update(const float deltaSeconds) {
    if (!open_) {
        return;
    }
    scrollY_ += kAutoScrollPxPerSec * deltaSeconds;
    const float maxScroll = std::max(0.0f, contentHeight_ - static_cast<float>(kLogicalHeight) + 80.0f);
    if (scrollY_ > maxScroll) {
        scrollY_ = maxScroll;
    }
}

void IntroductionScreen::drawUiText(SDL_Renderer *renderer, LauncherChrome *chrome, const char *text,
                                    const float x, const float y, const SDL_Color color) const {
    if (chrome && chrome->font().isReady()) {
        chrome->font().drawText(renderer, text, x, y, color);
        return;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, x, y, text);
}

void IntroductionScreen::rebuildWrappedLines(LauncherChrome *chrome) {
    wrappedLines_.clear();
    const bool useFont = chrome && chrome->font().isReady();
    float y = kPaddingTop;
    int lineH = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + static_cast<int>(kLineSpacing);

    for (const char *paragraph : kStoryParagraphs) {
        std::vector<std::string> chunk;
        if (useFont) {
            chunk = WrapParagraphFont(paragraph, chrome->font(), kMaxTextWidth);
        } else {
            chunk = WrapParagraphAscii(paragraph, 78);
            lineH = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + static_cast<int>(kLineSpacing);
        }

        if (useFont && !chunk.empty() && !chunk.front().empty()) {
            int dummyW = 0;
            int h = 0;
            chrome->font().measureText("Ay", &dummyW, &h);
            if (h > 0) {
                lineH = h + static_cast<int>(kLineSpacing);
            }
        }

        for (const std::string &line : chunk) {
            wrappedLines_.push_back(line);
            y += static_cast<float>(lineH);
        }
        y += kParagraphGap;
    }

    contentHeight_ = y + 120.0f;
    linesDirty_ = false;
}

void IntroductionScreen::Render(SDL_Renderer *renderer, LauncherChrome *chrome) {
    if (!open_) {
        return;
    }

    if (linesDirty_) {
        rebuildWrappedLines(chrome);
    }

    if (chrome) {
        chrome->renderBackground(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 8, 10, 18, 255);
        SDL_RenderClear(renderer);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_FRect dim{0.0f, 0.0f, static_cast<float>(kLogicalWidth), static_cast<float>(kLogicalHeight)};
    SDL_RenderFillRect(renderer, &dim);

    const SDL_Color textTitle{222, 158, 0, 255};
    const SDL_Color textBody{235, 235, 245, 255};
    const SDL_Color textHint{160, 170, 190, 255};

    drawUiText(renderer, chrome, "L'Haruspice et la Quatrieme Prophetie", kPaddingX, 28.0f, textTitle);

    const bool useFont = chrome && chrome->font().isReady();
    int lineH = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + static_cast<int>(kLineSpacing);
    if (useFont) {
        int dummyW = 0;
        int h = 0;
        chrome->font().measureText("Ay", &dummyW, &h);
        if (h > 0) {
            lineH = h + static_cast<int>(kLineSpacing);
        }
    }

    const float viewTop = kPaddingTop;
    const float viewBottom = static_cast<float>(kLogicalHeight) - 56.0f;
    float y = kPaddingTop - scrollY_;
    for (std::size_t i = 0; i < wrappedLines_.size(); ++i) {
        const std::string &line = wrappedLines_[i];
        if (y + static_cast<float>(lineH) >= viewTop && y <= viewBottom) {
            const SDL_Color color =
                (i == 0 && line == "Conte de la Quatrieme Venue") ? textTitle : textBody;
            drawUiText(renderer, chrome, line.c_str(), kPaddingX, y, color);
        }
        y += static_cast<float>(lineH);
        if (y > viewBottom + static_cast<float>(lineH)) {
            break;
        }
    }

    drawUiText(renderer, chrome, "Defilement auto — Fleches / molette — Esc ou clic : fermer", kPaddingX,
               static_cast<float>(kLogicalHeight) - 36.0f, textHint);

    if (chrome) {
        chrome->renderBanner(renderer);
    }
}

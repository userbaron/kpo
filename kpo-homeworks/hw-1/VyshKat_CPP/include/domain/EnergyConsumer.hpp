#ifndef ENERGYCONSUMER_HPP
#define ENERGYCONSUMER_HPP

class IEnergyConsumer {
public:
    virtual ~IEnergyConsumer() = default;
    [[nodiscard]] virtual double dailyEnergyKWh() const noexcept = 0;
};

class EnergyConsumer : public IEnergyConsumer {
public:
    explicit EnergyConsumer(double dailyKWh);
    [[nodiscard]] double dailyEnergyKWh() const noexcept override;

private:
    double dailyKWh_;
};

#endif
